#include "httoken.h"
#include <stdlib.h>
#include <string.h>

tokpool_t *tkPoolInit(){
	tokpool_t *p = malloc(sizeof(tokpool_t));
	p->tpBlk = calloc(DEFAULT_CHUNK, TOKEN_SZ);

	p->chunks = DEFAULT_CHUNK;

	p->tpFreeBlk = p->tpBlk + DEFAULT_CHUNK_SIZE; //growing downwards
	return p;
}

void tpEmplace(token_t tk, tokpool_t *tp){
	tp->tpFreeBlk->tokType = tk.tokType;
	tp->tpFreeBlk->tokStr = tk.tokStr;
	tp->tpFreeBlk->tokStrLen = tk.tokStrLen;
	tp->tpFreeBlk -= TOKEN_SZ;
	if(tp->tpFreeBlk <= tp->tpBlk){
		printf("WE HAVE EXCEEDED THE POOL AND NEED A REALLOC\n");
		size_t newChunk = tp->chunks + DEFAULT_CHUNK;
		size_t newChunkSize = newChunk * TOKEN_SZ;
		token_t *newBlk = realloc(tp->tpBlk, newChunkSize);
		tp->tpBlk = newBlk;
		tp->chunks = newChunk;
		tp->tpFreeBlk = tp->tpBlk + newChunkSize;
	}
	return;
	//I think to ensure this all works the way I need it to, I'll need to write my own realloc
}

	














token_t *allocateEmptyToken(){
	token_t *token = malloc(sizeof(token_t));
	token->tokType = tok_TBD;
	token->tokStrLen = -1;
	token->tokStr = NULL;
	return token;
}

lexerNode_t *createNode(char *s){
	lexerNode_t *node = malloc(sizeof(lexerNode_t));
	node->ll_tok = allocateEmptyToken();
	node->ll_tok->tokStr = strdup(s);
	node->ll_next = NULL;
	return node;
}


void freeIdentToken(token_t *ident){
	if(ident->tokType != tok_identifier){
		return;
	}
	if(ident->tokStr){
		free(ident->tokStr);
		ident->tokStr = NULL;
	}
	return;
}

void freeToken(token_t **tk){
	if (__builtin_expect(tk == NULL || *tk == NULL, 0)) {
		return;
	}
	if((*tk)->tokStr){
		free((*tk)->tokStr);
		(*tk)->tokStr = 0x00;
	}
	free(*tk);
	*tk = 0x00;
	return;
}



void indescriminateMemoryExtermination(lexerNode_t *head){
	lexerNode_t *cur = head;
	while(cur != 0x00){
		lexerNode_t *next = cur->ll_next;
		if(cur->ll_tok){
			freeToken(&cur->ll_tok);
		}
		free(cur);
		cur = next;
	}
}

