#include "httoken.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

tokpool_t *globalPool = NULL;
tokpool_t *tpPoolInit(){
	tokpool_t *p = malloc(sizeof(tokpool_t));
	p->tpBlk = calloc(DEFAULT_CHUNK, TOKEN_SZ);

	p->chunks = DEFAULT_CHUNK;
	p->tpStack.tpFree = malloc(sizeof(uint16_t) * DEFAULT_CHUNK);
	p->tpStack.tpStackPtr = 0;
	p->tpStack.tpStackSz = DEFAULT_CHUNK;

	for(int i = 0; i < p->chunks; i++){
		p->tpStack.tpFree[i] = i;
	}
	p->tpStack.tpStackPtr = p->tpStack.tpStackSz;

	return p;
}

token_t *tpStkPop(tokpool_t *tp){
	token_t *tk = &tp->tpBlk[tp->tpStack.tpStackPtr];
	tk->tokType = tok_TBD;
	tk->tokStrLen = -1;
	tk->tokStr = NULL;
	tp->tpStack.tpStackPtr--;
	if(tp->tpStack.tpStackPtr == 0){
		printf("EMERGENCY REALLOC NEEDED BUT NOT POSSIBLE\n");
		//TODO: realloc the pool
	}
	return tk;
}

int tpStkIndex(token_t *tk, tokpool_t *tp){
	ptrdiff_t i = tk - tp->tpBlk;
	if (i < 0 || (size_t)i >= tp->chunks){
		return -1;
	}
	return (int)i;
}

void tpStkPush(token_t *tk, tokpool_t *tp){
	uint16_t i = tpStkIndex(tk, tp);
	tp->tpStack.tpStackPtr++;
	tp->tpStack.tpFree[tp->tpStack.tpStackPtr] = i;
	if(tp->tpStack.tpStackPtr == tp->tpStack.tpStackSz){
		printf("STACK OVERFLOW\n");
		//TODO: realloc the stack
	}
	return;
}

//TODO:
/* Both the pop and push instructions should probably be bare assembly
   I'm using the stack approach to reduce overhead and fragmentation
   I can reduce CPU overhead and CPU usgae with assembly than compiled C */
	






void freeToken(token_t *tk, tokpool_t *tp){
	if (__builtin_expect(tk == NULL, 0)) {
		return;
	}
	if(tk->tokStr){
		free(tk->tokStr);
	}
	tk->tokType = tok_INVALID;
	tpStkPush(tk, tp);
	return;
}

lexerNode_t *createNode(char *s, tokpool_t *tp){
	lexerNode_t *node = malloc(sizeof(lexerNode_t));
	node->ll_tok = tpStkPop(tp);
	node->ll_tok->tokStr = strdup(s);
	node->ll_next = NULL;
	return node;
}








