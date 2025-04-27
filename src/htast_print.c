#include <stdio.h>
#include <stdlib.h>
#include "httoken.h"
#include "htast.h"
#include "htast_print.h"

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

void printDef(struct stmt_s *d){
	if(d->stmtType == stmtDef){
		printf("Call Definition Statement\n");
		printf("Call name : \n");
		printf("%s\n", d->l.callDef.callName->tokStr);
		printf("%d number of args\n", d->l.callDef.argCount);
		printf("Args : \n");
		for(int i = 0; i < d->l.callDef.argCount; i++){
			printf("%s\n", d->l.callDef.args[i]->tokStr);
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

