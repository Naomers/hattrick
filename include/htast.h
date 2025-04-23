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
	reprDotAccess, //x.var
	reprBrackAccess, //x[var]
};

struct repr_s {
	enum repr_e reprType;
	union {
		struct {
			token_t *id;
		}ident;
		struct {
			token_t *lit;
		}literal;
		struct {
			int atSign;
			token_t *callName;
			int argCount;
			struct repr_s **args;
		}call;
		struct {
			struct repr_s *lhs; //lefthand side
			token_t *oper;
			struct repr_s *rhs;
		}oper;
		struct {
			token_t *id;
			struct repr_r *access;
		}dotAcc;
		struct {
			token_t *id;
			struct repr_r *access;
		}brackAcc;


	}l;
};

struct stmt_s {
	enum stmt_e stmtType;
	union {
		struct {
			struct repr_s **targets;
			struct repr_s *assignment;
			int targCount;
		}ass;
		struct {
			struct repr_s *rv;
		}ret;
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

void printreplit(struct repr_s *r);
void printrepident(struct repr_s *r);
void printOperation(struct repr_s *r);
void printrepCall(struct repr_s *r);

struct repr_s *parseOperKnownLHS(lexerNode_t *lex, struct repr_s *lhs);




#endif
