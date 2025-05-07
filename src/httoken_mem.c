#include "httoken.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

tokpool_t *tpInit(){
	tokpool_t *p = malloc(sizeof(tokpool_t));
	p->tpBlk = calloc(DEFAULT_CHUNK, TOKEN_SZ);

	p->chunks = DEFAULT_CHUNK;

	p->tpTop = &p->tpBlk[DEFAULT_CHUNK];
	p->tpCursor = &p->tpBlk[0];
	for(int i = 0; i < DEFAULT_CHUNK; i++){
		p->tpBlk[i].tokType = tok_TBD;
	}
	return p;
}

token_t *tpReturnCurrentTok(tokpool_t *tp){
	if(tp->tpCursor->tokType != -1){
		tp->tpCursor++;
		tpReturnCurrentTok(tp);
	}
	return tp->tpCursor;
}





/*
lexerNode_t *createNode(char *s){
	printf("%s\n", s);
	lexerNode_t *node = malloc(sizeof(lexerNode_t));
	node->ll_tok = tpStkPop(g_tkMemPool);
	node->ll_tok->tokStr = strdup(s);
	node->ll_next = NULL;
	return node;
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
}*/
