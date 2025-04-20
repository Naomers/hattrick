#ifndef PARSER_H
#define PARSER_H

#include "httoken.h"
#include <stdio.h>
#include <stdlib.h>

// Token helpers
enum tokenType_e currentTokenType(lexerNode_t *cur);
token_t *nextToken(lexerNode_t *cur);
enum tokenType_e nextTokenType(lexerNode_t *cur);
int match(lexerNode_t *cur, enum tokenType_e e);

// Syntax error
void syntaxError(const char *msg);

// Statement Types
enum stmt_type {
	stmt_assignment,
	stmt_binop,
	stmt_comparison,
	stmt_funcCall,
	stmt_funcDef,
	stmt_ifStmt,
	stmt_identifier,
	stmt_literal,
	stmt_funcArgs,
};

// Forward declare stmt_t
typedef struct stmt_s stmt_t;

// Statement structure
struct stmt_s {
	enum stmt_type stmtType;
	union {
		struct {
			token_t *targets;
			token_t *assignment;
			stmt_t *value;
		} assignment;

		struct {
			stmt_t *left;
			token_t *operation;
			stmt_t *right;
		} binop;

		struct {
			int atSign;
			token_t *functionName;
			stmt_t **args;
			int argCount;
		} funcCall;

		struct {
			token_t *id;
		} identifier;

		struct {
			token_t *lit;
		} literal;

		struct { 
			stmt_t *arg;
		} funcArg;
	} v;
};

// Parser functions
stmt_t *parseFactor(lexerNode_t *cur);
int isAssignment(lexerNode_t *cur);
int isFunctionCall(lexerNode_t *cur);
int funcArgCount(lexerNode_t *cur);
stmt_t *parseFunctionCall(lexerNode_t *cur);

// Helper Macros
#define ADVANCE(cur)        ((cur) && (cur)->ll_next ? (cur)->ll_next : NULL)

#endif // PARSER_H

