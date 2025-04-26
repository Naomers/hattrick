#ifndef HTAST_HELPER_H
#define HTAST_HELPER_H

static inline void ADVANCE(register lexerNode_t **cur){
	asm volatile(
		"mov (%0), %%rax\n\t"     // rax = *cur
		"mov 8(%%rax), %%rcx\n\t" // rcx = (*cur)->ll_next
		"mov %%rcx, (%0)"	  // *cur = rcx 
		:
		: "r" (cur)
		: "rax", "rcx", "memory"
	);
	// This will be HELL to debug if I ever change the lexerNode_t struct
	// making the 8 offset no longer valid.
}


enum tokenType_e tkType(register lexerNode_t *cur){
	int result;
	asm volatile(
		"mov %[cur], %%rax\n\t"
		"mov (%%rax), %%rax\n\t"
		"test %%rax, %%rax\n\t"
		"movl $-2, %[result]\n\t"
		"je 1f\n\t"
		"mov (%%rax), %%ecx\n\t"
		"mov %%ecx, %[result]\n"
		"1:"
		: [result] "=r" (result)
		: [cur] "r" (cur)
		: "rax", "eax"
	);
	return result;
	//0.17% faster. Could be better. Not worth my time worrying about right now.
}

#define NEXT(cur) ((cur) && (cur)->ll_next ? (cur)->ll_next : NULL)

token_t *nextTk(lexerNode_t *cur){
	ADVANCE(&cur);
	if(cur->ll_tok){
		return cur->ll_tok;
	}
	return NULL;
}

enum tokenType_e nextTkType(lexerNode_t *cur){
	ADVANCE(&cur);
	if(!cur){
		return tok_INVALID;
	}
	return tkType(cur);
} 

token_t *tk(lexerNode_t *cur){
	if(cur->ll_tok){
		return cur->ll_tok;
	}
	return NULL;
}

int match(lexerNode_t *cur, enum tokenType_e e){
	if(tkType(cur) == e){
		return 1;
	}
	return 0;
}

int matchNext(lexerNode_t *cur, enum tokenType_e e){
	enum tokenType_e nxtE = nextTkType(cur);
	if(nxtE == e){
		return 1;
	}
	return 0;
}

int isTypeInLex(lexerNode_t *lex, enum tokenType_e e){
	for(;;){
		if(lex == NULL){
			return 0;
		}
		if(match(lex, e)){
			return 1;
		}
		ADVANCE(&lex);
	}
}

void advanceTo(lexerNode_t **lex, enum tokenType_e to){
	lexerNode_t *cur = *lex;
	for(;;){
		if(cur == NULL){
			return;
		}
		if(match(cur, to)){
			return;
		}
		ADVANCE(lex);
		cur = *lex;
	}
}

#endif
