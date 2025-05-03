#include "htast_mem.h"

void freeRep(struct repr_s *r){
	if(!r){
		return;
	}
	switch(r->reprType){
		case reprIdent:
			freeToken(r->l.ident.id, g_tkMemPool);
			break;
		case reprCall:
			freeToken(r->l.call.callName, g_tkMemPool);
			for(int i = 0; i != r->l.call.argCount; i++){
				freeRep(r->l.call.args[i]);
			}
			break;
		case reprLiteral:
			freeToken(r->l.literal.lit, g_tkMemPool);
			break;
		case reprOperation:
			freeRep(r->l.oper.lhs);
			freeRep(r->l.oper.rhs);
			freeToken(r->l.oper.oper, g_tkMemPool);
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
			freeToken(s->l.callDef.callName, g_tkMemPool);
			for(int i = 0; i != s->l.callDef.argCount; i++){
				freeToken(s->l.callDef.args[i], g_tkMemPool);
			}
			free(s->l.callDef.args);
			break;
		default:
			printf("error in freeing\n");
	}
	free(s);
	s = NULL;
}

