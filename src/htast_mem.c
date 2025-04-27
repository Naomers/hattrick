#include "htast_mem.h"

void freeRep(struct repr_s *r){
	if(!r){
		return;
	}
	switch(r->reprType){
		case reprIdent:
			freeIdentToken(r->l.ident.id);
			break;
		case reprCall:
			freeIdentToken(r->l.call.callName);
			for(int i = 0; i != r->l.call.argCount; i++){
				freeRep(r->l.call.args[i]);
			}
			break;
		case reprLiteral:
			freeIdentToken(r->l.literal.lit);
			break;
		case reprOperation:
			freeRep(r->l.oper.lhs);
			freeRep(r->l.oper.rhs);
			freeToken(&r->l.oper.oper);
			break;
		default:
			printf("error\n");
	}
	free(r);
	r = NULL;
}

void freeStmt(struct stmt_s *s){
	switch(s->stmtType){
		case stmtAssign:
			for(int i = 0; i != s->l.ass.targCount; i++){
				freeRep(s->l.ass.targets[i]);
			}
			free(s->l.ass.targets);
			freeRep(s->l.ass.assignment);
			break;
		case stmtReturn:
			freeRep(s->l.ret.rv);
			break;
		case stmtDef:
			freeToken(&s->l.callDef.callName);
			for(int i = 0; i != s->l.callDef.argCount; i++){
				freeToken(&s->l.callDef.args[i]);
			}
			free(s->l.callDef.args);
			break;
		default:
			printf("error in freeing\n");
	}
	free(s);
	s = NULL;
}

