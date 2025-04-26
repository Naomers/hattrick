#include "httoken.h"
#include <stdlib.h>
#include <string.h>

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

void freeToken(token_t *tk){
	if (__builtin_expect(tk == NULL, 0)) {
		return;
	}
	if(tk->tokStr){
		free(tk->tokStr);
	}
	free(tk);
	tk = NULL;
	return;
}



void indescriminateMemoryExtermination(lexerNode_t *head){
	lexerNode_t *cur = head;
	while(cur != NULL){
		lexerNode_t *next = cur->ll_next;
		if(cur->ll_tok){
			if(cur->ll_tok->tokStr){
				free(cur->ll_tok->tokStr);
				cur->ll_tok->tokStr = NULL;
			}
			free(cur->ll_tok);
			cur->ll_tok = NULL;
		}
		free(cur);
		cur = next;
	}
}

