#ifndef HTAST_H
#define HTAST_H


enum stmt_e {
	stmtAssign,
	stmtDef,
	stmtReturn,

	stmtExpr, //if an expr or repr is a stmt, like funcCall()
};

enum repr_e {
	//repr are expressions that represent a value
	reprIdent,
	reprLiteral,
	reprCall,
	reprOperation,
	reprCondition,
};

struct repr_s {
	enum repr_e reprType;
	union {
		struct {
			token_t *id;
		}ident;
		struct {
			int atSign;
			token_t *callName;
			int argCount;
			struct repr_s **args;
		}call;

	}l;
};

struct stmt_s {
	enum stmt_e stmtType;
	union {
		struct {
			struct repr_s *repr;
		}repStmt;
	}l;
};


int isCall(lexerNode_t *lex);
int callCountArgs(lexerNode_t *lex);
struct repr_s *parseIdent(lexerNode_t *cur);
struct repr_s *parseRepr(lexerNode_t *lex);
struct repr_s *parseCall(lexerNode_t *lex);





#endif
