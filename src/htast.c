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

int isOperation(lexerNode_t *lex){
	if(isTypeInLex(lex, tok_op_plus)  ||
	   isTypeInLex(lex, tok_op_minus) ||
	   isTypeInLex(lex, tok_op_mult)  ||
	   isTypeInLex(lex, tok_op_div)){
	   	return 1;
	}
	//TODO:
	//this is a simplistic as shit mthod of checking and it kinda sucks ass. Refine it lots
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

void printreplit(struct repr_s *r){
	if(r->reprType == reprLiteral){
		printf("This is an AST node for a literal.\n");
		switch(r->l.literal.lit->tokType){
			case tok_literal_str:
				printf("Literal string\n");
				break;
			case tok_literal_num:
				printf("literal number\n");
		}
		printf("literal is - %s\n", r->l.ident.id->tokStr);
	}
}


void printrepident(struct repr_s *r){
	if(r->reprType == reprIdent){
		printf("This is an AST node for an identifier.\n");
		printf("ident is - %s\n", r->l.ident.id->tokStr);
	}
}

void printOperation(struct repr_s *r){
	if(r->reprType == reprOperation){
		printf("This is an AST node for an operation.\n");
		struct repr_s *s = r->l.oper.lhs;
gotoPrintSide:
		switch(s->reprType){
			case reprCall:
				printrepCall(s);
				break;
			case reprIdent:
				printrepident(s);
				break;
			case reprLiteral:
				printreplit(s);
				break;
			case reprOperation:
				printOperation(s);
				break;
		}
		if(s == r->l.oper.lhs){
			s = r->l.oper.rhs;
			goto gotoPrintSide;
		}
	}
}

void printrepCall(struct repr_s *r){
	if(r->reprType == reprCall){
		printf("This is a function call\n");
		printf("function is named : %s\n", r->l.call.callName->tokStr);
		printf("Function has %d arguments\n", r->l.call.argCount);
		for(int i = 0; i != r->l.call.argCount; i++){
			switch(r->l.call.args[i]->reprType){
				case reprCall:
					printrepCall(r->l.call.args[i]);
					break;
				case reprIdent:
					printrepident(r->l.call.args[i]);
					break;
				case reprLiteral:
					printreplit(r->l.call.args[i]);
					break;
				case reprOperation:
					printOperation(r->l.call.args[i]);
					break;
			}
		}
	}
}

void printRep(struct repr_s *r){
	switch(r->reprType){
		case reprCall:
			printrepCall(r);
			return;
		case reprIdent:
			printrepident(r);
			return;
		case reprLiteral:
			printreplit(r);
			return;
		case reprOperation:
			printOperation(r);
			return;
	}
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
	return s;
}

void printReturn(struct stmt_s *r){
	if(r->stmtType == stmtReturn){
		printf("Return statement\n");
		printf("Return value : \n");
		printRep(r->l.ret.rv);
	}
}

void printAssignment(struct stmt_s *a){
	if(a->stmtType == stmtAssign){
		printf("Assignment statement\n");
		printf("Assignment targets : \n");
		for(int i = 0; i != a->l.ass.targCount; i++){
			printRep(a->l.ass.targets[i]);
		}
		printf("Assign value is\n");
		printRep(a->l.ass.assignment);
	}
}



int main(){
	char buff[1024];
	fgets(buff, sizeof(buff), stdin);

	lexerNode_t *tokens = httokenize(buff);
	determineTokenTypes(tokens);

	struct stmt_s *s = parse(tokens);
	printAssignment(s);
	//printReturn(s);

	indescriminateMemoryExtermination(tokens);

	//debugWalk(tokens);
}

//TODO: organize this whole mess of a file!!!
