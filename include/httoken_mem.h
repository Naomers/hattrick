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
	struct stack {
		uint16_t *tpFree; //Array of free indices for the tpBlk array
		uint16_t tpStackPtr; //stack pointer for the tpStack
		size_t tpStackSz;
	}tpStack; //The free stack is first so its the fastest to address

	ALIGN_16 token_t *tpBlk; 

	size_t chunks;
}tokpool_t;

extern tokpool_t *globalPool;

tokpool_t *tpPoolInit();
token_t *tpStkPop(tokpool_t *tp);
void tpStkPush(token_t *tk, tokpool_t *tp);


void freeToken(token_t *tk, tokpool_t *tp);
lexerNode_t *createNode(char *s, tokpool_t *tp);

#endif
