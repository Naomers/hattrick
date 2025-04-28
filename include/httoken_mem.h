#ifndef HTTOKEN_MEM
#define HTTOKEN_MEM

#include "httoken.h"

#define TOKEN_SZ sizeof(token_t)
//#define DEFAULT_CHUNK 32
#define DEFAULT_CHUNK 1
#define DEFAULT_CHUNK_SIZE TOKEN_SZ * DEFAULT_CHUNK

typedef struct tokpool_s {
	token_t *tpFreeBlk;
	//freeBlk at the top for faster accessing
	token_t *tpBlk;
	size_t chunks;
}tokpool_t;

tokpool_t *tkPoolInit();
void tpEmplace(token_t tk, tokpool_t *tp);



void indescriminateMemoryExtermination(lexerNode_t *head);
void determineTokenTypes(lexerNode_t *head);
void freeIdentToken(token_t *ident);
void freeToken(token_t **tk);
token_t *allocateEmptyToken(void);
lexerNode_t *createNode(char *s);

#endif
