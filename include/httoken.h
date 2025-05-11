#ifndef HTTOKEN_H
#define HTTOKEN_H

enum tokType_e {
	TOK_INVALID = -2,
	TOK_TBD = 0,
	
	TOK_ID,
	TOK_LITERAL_NUM,
	TOK_LITERAL_STR,
	
	TOK_OP_PLUS,
	TOK_OP_MINUS,
	TOK_OP_MULT,
	TOK_OP_DIV,
	TOK_OP_ASSIGNMENT,
	TOK_OP_ATSIGN,
	TOP_OP_DOT,

};

typedef struct token_s {
	enum tokType_e tokType;
	char *tokStr;
	int tokStrLen;
}token_t;

typedef struct lexerNode_s {
	token_t *llTok;
	struct lexerNode_s *llNext;
}lexerNode_t;



#endif
