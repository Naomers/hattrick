#include "httoken.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

tokpool_t *g_tkMemPool = NULL;

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
	p->tpStack.tpStackPtr = p->tpStack.tpStackSz - 1;

	return p;
}

int tpStkIndex(token_t *tk, tokpool_t *tp){
	ptrdiff_t i = tk - tp->tpBlk;
	if (i < 0 || (size_t)i >= tp->chunks){
		return -1;
	}
	return (int)i;
}

token_t *tpPoolRealloc(token_t *oldBlk, size_t oSz, size_t nSz){
	token_t *newPool = calloc(1, nSz);
	size_t copySize = oSz < nSz? oSz: nSz;
	memcpy(newPool, oldBlk, copySize);
	free(oldBlk);
	return newPool;
}




token_t *tpStkPop(tokpool_t *tp){
	printf("Using index %d\n", tpStkIndex(&tp->tpBlk[tp->tpStack.tpFree[tp->tpStack.tpStackPtr]], tp));
	token_t *tk = &tp->tpBlk[tp->tpStack.tpFree[tp->tpStack.tpStackPtr]];
	tk->tokType = tok_TBD;
	tk->tokStrLen = -1;
	tk->tokStr = NULL;
	tp->tpStack.tpStackPtr--;
	if(tp->tpStack.tpStackPtr == 1){
		//TODO: realloc the pool
		tp->tpBlk = tpPoolRealloc(tp->tpBlk, tp->chunks,(tp->chunks + DEFAULT_CHUNK) * TOKEN_SZ);
		for(int i = 0; i < DEFAULT_CHUNK; i++){
			//printf("USINGNGGNNGNGNG %d\n\n", i + tp->chunks);
			//tpStkPush(&tp->tpBlk[i + tp->chunks], tp);
			tp->tpStack.tpStackPtr++;
			tp->tpStack.tpFree[i + tp->chunks];
			if (tp->tpStack.tpStackPtr + 1 >= tp->tpStack.tpStackSz){
				break;
			}

		}
		tp->chunks += DEFAULT_CHUNK;
	}

	return tk;
}
void tpStkPush(token_t *tk, tokpool_t *tp){
	if (tp->tpStack.tpStackPtr + 1 >= tp->tpStack.tpStackSz) {
		printf("STACK OVERFLOW\n");
		//TODO: realloc the stack
		return;
	}

	uint16_t i = tpStkIndex(tk, tp);
	//printf("%d ADDING TO STACK INDEX\n", i);
	tp->tpStack.tpStackPtr++;
	tp->tpStack.tpFree[tp->tpStack.tpStackPtr] = i;
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
		tk->tokStr = NULL;
	}
	tpStkPush(tk, tp);
	tk = NULL;
	return;
}

lexerNode_t *createNode(char *s){
	printf("%s\n", s);
	lexerNode_t *node = malloc(sizeof(lexerNode_t));
	node->ll_tok = tpStkPop(g_tkMemPool);
	node->ll_tok->tokStr = strdup(s);
	node->ll_next = NULL;
	return node;
}


void freePool(tokpool_t *tp){
	if(tp->tpStack.tpFree){
		free(tp->tpStack.tpFree);
	}
	free(tp->tpBlk);
	free(tp);
}

void freeLex(lexerNode_t *head){
	lexerNode_t *cur = head;
	while(cur != NULL){
		lexerNode_t *next = cur->ll_next;
		if(cur->ll_tok){
			freeToken(cur->ll_tok, g_tkMemPool);
		}
		free(cur);
		cur = next;
	}
}
