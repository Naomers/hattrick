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

token_t *tpRealloc(tokpool_t *tp, size_t chunks){
	token_t *nb = calloc(chunks, TOKEN_SZ);

	size_t oldChunk_sz = tp->chunks * TOKEN_SZ;
	size_t newChunk_sz = chunks * TOKEN_SZ;

	size_t usedBytes = oldChunk_sz - (tp->tpFreeBlk - tp->tpBlk);

	memcpy(nb + newChunk_sz - usedBytes,
	       tp->tpFreeBlk,
	       usedBytes);

	free(tp->tpBlk);
	tp->tpBlk = nb;
	tp->chunks = chunks;
	tp->tpFreeBlk = nb + newChunk_sz - usedBytes;
}




void tpEmplace(token_t tk, tokpool_t *tp){
	tp->tpFreeBlk->tokType = tk.tokType;
	tp->tpFreeBlk->tokStr = tk.tokStr;
	tp->tpFreeBlk->tokStrLen = tk.tokStrLen;
	tp->tpFreeBlk -= TOKEN_SZ;
	if(tp->tpFreeBlk <= tp->tpBlk){
		printf("WE HAVE EXCEEDED THE POOL AND NEED A REALLOC\n");
		tpRealloc(tp, tp->chunks + DEFAULT_CHUNK);
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

