#include <stdio.h>
#include <stdlib.h>
#include "httoken.h"
#include "htast.h"
#include "htast_helper.h"

/* Naming convention for clarity:
/  a lexerNode_t pointer is called cur if only this and the next one are used. 
/  a lexerNode_t pointer is called lex if it's used as an entire lexed and tokenized stmt*/

int isCall(lexerNode_t *lex){
	if(match(lex, tok_op_atSign)){
		if(!matchNext(lex, tok_identifier)){
			return -1;
		}
	//	isCall(next(lex));
		ADVANCE(&lex);
	}
	if(!match(lex, tok_identifier)){
		return -1;
	}
	if(!matchNext(lex, tok_op_lpar)){
		return -1;
	}
	enum tokenType_e crnt = tkType(lex);
	for(;;){
		crnt = tkType(lex);
		switch(crnt){
			case tok_op_rpar:
				return 1;
			case tok_INVALID:
				return -1;
			default:
				ADVANCE(&lex);
		}
		if(lex == NULL){
			return -1;
		}
	}
}

int callCountArgs(lexerNode_t *lex){
	advanceTo(&lex, tok_op_lpar);
	if(matchNext(lex, tok_op_rpar)){
		return 0;
	}
	int aCount = 1;
	for(;;){
		if(match(lex, tok_op_rpar)){
			return aCount;
		}
		if(match(lex, tok_op_comma)){
			switch(nextTkType(lex)){
				case tok_identifier:
				case tok_literal_num:
				case tok_literal_str:
					aCount++;
					break;
				case tok_op_rpar:
					return aCount;
				default:
					printf("ERROR in TEXT STREAM\n");
					return -1;
			}
		}
		ADVANCE(&lex);
	}
	return aCount;
}
		



struct repr_s *parseIdent(lexerNode_t *cur){
	struct repr_s *r = malloc(sizeof(struct repr_s));
	r->reprType = reprIdent;
	r->l.ident.id = tk(cur);
	return r;
}


struct repr_s *parseRepr(lexerNode_t *lex){
	struct repr_s *r = NULL;
uglyLazyGotoHack:     // I'm so sorry Dijkstra. This is awful.
	switch(tkType(lex)){
		case tok_op_lpar:
			ADVANCE(&lex);
			goto uglyLazyGotoHack;
		case tok_identifier:
			if(isCall(lex) == 1){
				printf("WE GOT A FUNCTION IN A FUNCTION\n");
				parseCall(lex);
			}
			switch(nextTkType(lex)){
				case tok_INVALID:
					return r;
				default:
					r = parseIdent(lex);
					return r;
			}
	}
	return r;
}


struct repr_s *parseCall(lexerNode_t *lex){ 
	struct repr_s *r = malloc(sizeof(struct repr_s));
	r->reprType = reprCall;
	if(match(lex, tok_op_atSign)){
		r->l.call.atSign = 1;
		ADVANCE(&lex);
	}

	r->l.call.args = malloc(sizeof(struct repr_s) * callCountArgs(lex));
	r->l.call.argCount = callCountArgs(lex); //obviously this could be optimized
	// I shouldn't run these traversal functions more than need be
	// but this is so rough idgaf
	int argOff = 0;

	if(match(lex, tok_identifier)){
		r->l.call.callName = tk(lex);
		ADVANCE(&lex);
	}else {
		printf("WHAT IS HAPPENING ????\n");
		free(r);
		return NULL;
	}
	if(match(lex, tok_op_lpar)){
		enum tokenType_e crnt = tkType(lex);
		if(!isTypeInLex(lex, tok_op_rpar)){
			free(r);
			printf("ERERERRERERRERR\n");
			return NULL;
		}
		for(;;){
			if(match(lex, tok_op_rpar)){
				return r;
			}
			r->l.call.args[argOff] = parseRepr(lex);
			argOff++;
			if(isTypeInLex(lex, tok_op_comma)){
				advanceTo(&lex, tok_op_comma);
			}
			ADVANCE(&lex);
		}
	}

	return r;
}

void printrepident(struct repr_s *r){
	printf("%d\n", r->reprType);
	if(r->reprType == reprIdent){
		printf("This is an AST node for an identifier.\n");
		printf("ident is - %s\n", r->l.ident.id->tokStr);
	}
}

void printrepCall(struct repr_s *r){
	if(r->reprType == reprCall){
		printf("This is a function call\n");
		printf("function is named : %s\n", r->l.call.callName->tokStr);
		printf("Function has %d arguments\n", r->l.call.argCount);
		for(int i = 0; i != r->l.call.argCount; i++){
			printrepident(r->l.call.args[i]);
		}
	}
}








int main(){
	char buff[1024];
	fgets(buff, sizeof(buff), stdin);

	lexerNode_t *tokens = httokenize(buff);
	determineTokenTypes(tokens);

	//printf("%s\n", tokens->ll_tok->tokStr);
	debugWalk(tokens);
	//printf("%d\n", isCall(tokens));
	//parseRepr(tokens);
	if(isCall(tokens)){
		printf("FUNCTION!!!\n");
		struct repr_s *call = parseCall(tokens);
		printrepCall(call);
	}
}
