#include <stdio.h>
#include <stdlib.h>
#include "httoken.h"
#include "htast.h"
#include "htast_helper.h"
#include "htast_print.h"
#include "htast_mem.h"

/* Naming convention for clarity:
/  a lexerNode_t pointer is called cur if only this and the next one are used. 
/  a lexerNode_t pointer is called lex if it's used as an entire lexed and tokenized stmt*/

int isCall(lexerNode_t *lex){
	if(match(lex, tok_op_atSign)){
		if(!matchNext(lex, tok_identifier)){
			return -1;
		}
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

struct repr_s *parseLiteral(lexerNode_t *cur){
	struct repr_s *r = malloc(sizeof(struct repr_s));
	r->reprType = reprLiteral;
	r->l.literal.lit = tk(cur);
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
				r = parseCall(lex);
				advanceTo(&lex, tok_op_rpar);
				if(nextTkType(lex) == tok_op_plus){
					ADVANCE(&lex);
					struct repr_s *rlev2 = parseOperKnownLHS(lex, r);
					return rlev2;
				}//This is a hacky way of implementing this.
				// not perfect but it works for now
				return r;
			}
			switch(nextTkType(lex)){
				case tok_INVALID:
					r = parseIdent(lex);
					return r;
				case tok_op_plus:
				case tok_op_minus:
				case tok_op_div:
				case tok_op_mult:
					r = parseIdent(lex);
					ADVANCE(&lex);
					struct repr_s *rLevelTwo = parseOperKnownLHS(lex, r);
					return rLevelTwo;
				default:
					r = parseIdent(lex);
					return r;
			}
		case tok_literal_num:
		case tok_literal_str:
			switch(nextTkType(lex)){
				case tok_INVALID:
					return r;
				case tok_op_plus:
				case tok_op_minus:
				case tok_op_div:
				case tok_op_mult:
					r = parseLiteral(lex);
					ADVANCE(&lex);
					struct repr_s *rLevelTwo = parseOperKnownLHS(lex, r);
					return rLevelTwo;
				default:
					r = parseLiteral(lex);
					return r;
			}
	}
	return r;
}
//TODO: 
// parseRepr is insanely ugly and hard to read
// this could easily be made more modular and more readable
// that's not even mentioning how unoptimized and garbo it is 

struct repr_s *parseOperation(lexerNode_t *lex){
	struct repr_s *lhs = parseRepr(lex);
	if(lhs == NULL){
		return NULL;
	}
	token_t *operator = tk(lex);
	ADVANCE(&lex);
	struct repr_s *rhs = parseRepr(lex);
	if(rhs == NULL) {
		free(lhs);
		return NULL;
	}
	struct repr_s *r = malloc(sizeof(struct repr_s));
	r->reprType = reprOperation;
	r->l.oper.lhs = lhs;
	r->l.oper.oper = operator;
	r->l.oper.rhs = rhs;

	return r;
}

struct repr_s *parseOperKnownLHS(lexerNode_t *lex, struct repr_s *lhs){
	token_t *operator = tk(lex);
	ADVANCE(&lex);
	struct repr_s *rhs = parseRepr(lex);
	if(rhs == NULL) {
		free(lhs);
		return NULL;
	}
	struct repr_s *r = malloc(sizeof(struct repr_s));
	r->reprType = reprOperation;
	r->l.oper.lhs = lhs;
	r->l.oper.oper = operator;
	r->l.oper.rhs = rhs;

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

int assignCountTargets(lexerNode_t *lex){
	int count = 0;
	while(1){
		switch(tkType(lex)){
			case tok_identifier:
				count++;
				break;
			case tok_op_comma:
				break;
			case tok_op_assignment:
				return count;
			default:
				printf("Syntax error\n");
				return -1;
		}
		ADVANCE(&lex);
	}
}//TODO: I don't like this because we have to iterate through the linked list twice
// once to count the number of targets, but a second time to write those targets into the assignemnt
// ast node.


struct stmt_s *parse(lexerNode_t *lex){
	struct stmt_s *s = malloc(sizeof(struct stmt_s));
	if(match(lex, tok_kw_return)){
		s->stmtType = stmtReturn;
		ADVANCE(&lex);
		s->l.ret.rv = parseRepr(lex);
	}
	if(isTypeInLex(lex, tok_op_assignment)){
		int tcount = assignCountTargets(lex);
		s->stmtType = stmtAssign;
		s->l.ass.targCount = tcount;
		s->l.ass.targets = malloc(tcount * sizeof(struct repr_s));//what a waste of RAM! lol
		for(int i = 0;; i++){
			switch(tkType(lex)){
				case tok_identifier:
					s->l.ass.targets[i] = parseIdent(lex);
					break;
				case tok_op_comma:
					break;
				case tok_op_assignment:
					ADVANCE(&lex);
					s->l.ass.assignment = parseRepr(lex);
					return s;
			}
			ADVANCE(&lex);
		}
	}
	if(isTypeInLex(lex, tok_op_lbrace)){
		if(!match(lex, tok_identifier)){
			printf("Syntax error\n");
			return NULL;
		}
		s->stmtType = stmtDef;
		s->l.callDef.callName = tk(lex);
		if(!isTypeInLex(lex, tok_op_lpar)){
			printf("Confused no par\n");
			return NULL;
		}
		int argCount = callCountArgs(lex);
		s->l.callDef.argCount = argCount;
		advanceTo(&lex, tok_op_lpar);
		ADVANCE(&lex);
		if(argCount){
			s->l.callDef.args = malloc(sizeof(token_t) * argCount);
			int i = 0;
			for(;;){
				switch(tkType(lex)){
					case tok_identifier:
						s->l.callDef.args[i] = tk(lex);
						i++;
						ADVANCE(&lex);
						break;
					case tok_op_comma:
						ADVANCE(&lex);
						break;
					case tok_op_rpar:
						return s;
					default:
						printf("SYNTAX ERROR CONFUSED\n");
						return NULL;
				}
				if(i == argCount){
					return s;
				}
			}
		}
	}
	return s;
}

#include "httoken_mem.h"

int main(){
	/*char buff[1024];
	fgets(buff, sizeof(buff), stdin);*/
	char *buff = "func(x, y, z){";

	lexerNode_t *tokens = httokenize(buff);
	determineTokenTypes(tokens);
	struct stmt_s *s = parse(tokens);
	printDef(s);
	freeLex(tokens);
	freeStmt(s);
	freePool(g_tkMemPool);
	

}

//TODO: organize this whole mess of a file!!!
