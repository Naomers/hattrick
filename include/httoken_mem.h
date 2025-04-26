#ifndef HTTOKEN_MEM
#define HTTOKEN_MEM

#include "httoken.h"

void indescriminateMemoryExtermination(lexerNode_t *head);
void determineTokenTypes(lexerNode_t *head);
void freeIdentToken(token_t *ident);
void freeToken(token_t *tk);
token_t *allocateEmptyToken(void);
lexerNode_t *createNode(char *s);

#endif
