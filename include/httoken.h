#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

enum tokenType_e {
	tok_INVALID = -2, // means there's been an error in the tokenization/lexing
			  // tells whatever this gets passed to, to cease its investigations

	tok_TBD = -1,     // This should probably be removed before release
			  // but this just means we don't know what the token type is yet
			  // To be determined!
	tok_identifier,
	tok_literal_num,
	tok_literal_str,


	//tok_op_* :
	//Operator tokens
	tok_op_plus,       // +
	tok_op_minus,      // -
	tok_op_mult,       // *
	tok_op_div,        // /
	tok_op_assignment, // =
	tok_op_atSign,     // @
	tok_op_dot,        // .
	tok_op_greater,    // >
	tok_op_less,       // <
	tok_op_dquote,     // "
	tok_op_squote,     // '
	tok_op_lpar,       // (
	tok_op_rpar,       // )
	tok_op_lbrack,     // [
	tok_op_rbrack,     // ]
	tok_op_comma,      // ,

	tok_comment,       // ;

	//tok_kw_* :
	//keyword tokens
	tok_kw_if,         // if
	tok_kw_true,       // true
	tok_kw_false,      // false
	tok_kw_null,       // null
};


typedef struct token_s {
    enum tokenType_e tokType;
    char *tokStr;
    int tokStrLen;
} token_t;

typedef struct lexerNode_s {
    token_t *ll_tok;
    struct lexerNode_s *ll_next;
} lexerNode_t;


// Function declarations
token_t *allocateEmptyToken(void);
enum tokenType_e singleCharReturn(char c);
char *substring(const char *s, int ws, int c);
int peekFor(const char *s, int ws, char peek);
int peekForAny(const char *s, int ws, const char *peeks);
lexerNode_t *createNode(char *s);
lexerNode_t *appendNode(lexerNode_t *head, lexerNode_t *newNode);
int isStringNumeric(const char *s);
enum tokenType_e getKeywordType(const char *str);
lexerNode_t *httokenize(const char *s);
void debugWalk(lexerNode_t *head);
void indescriminateMemoryExtermination(lexerNode_t *head);
void determineTokenTypes(lexerNode_t *head);

#endif // LEXER_H

