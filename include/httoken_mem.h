#ifndef HTTOKEN_MEM
#define HTTOKEN_MEM

#include <stdint.h>
#include "httoken.h"

#define TOKEN_SZ sizeof(token_t)
//#define DEFAULT_CHUNK 32
#define DEFAULT_CHUNK 16
#define DEFAULT_CHUNK_SIZE (TOKEN_SZ * DEFAULT_CHUNK)

#define ALIGN_16 __attribute__((aligned(16))) 

//might be a good idea to align the tpBlk as 16. Worth considering!

typedef struct tokpool_s {
	token_t *tpCursor;
	token_t *tpBlk; 
	token_t *tpTop;

	size_t chunks;
}tokpool_t;

extern tokpool_t *g_tkMemPool;

tokpool_t *tpInit();
/*
void freeToken(token_t *tk, tokpool_t *tp);
lexerNode_t *createNode(char *s);
void freeLex(lexerNode_t *head);
*/
#endif
