#ifndef HTAST_HELPER_H
#define HTAST_HELPER_H

void ADVANCE(lexerNode_t **cur) {
    if ((*cur)->ll_next) {
        *cur = (*cur)->ll_next;
	return;
    }
    *cur = NULL;
}

enum tokenType_e tkType(lexerNode_t *cur){
	if(cur->ll_tok){
		return cur->ll_tok->tokType;
	}
	return tok_INVALID;
}
// tkType should remain a function.
// when I wrote a macroized version, the inline code was actually much more bloated than a 'call' instruction.
// tkType was 0.23% slower macro-ized. Unless I cut corners (NOT A GOOD IDEA FOR AN ALREADY FRAGILE PARSER)
// tkType should remain a function.

#define NEXT(cur) ((cur) && (cur)->ll_next ? (cur)->ll_next : NULL)
/*
lexerNode_t *next(lexerNode_t *cur){
	if(cur->ll_next){
		return cur->ll_next;
	}
	return NULL;
}*/

#define next NEXT 
//TODO:
//too lazy to find n replace all instances of next() 
//do that for clarity

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

/*******************************************************************************************************
I'm strongly considering with the ast, instead of currently passing around a heap allocated
ast node (repr_s struct) to stuff like the helper functions I make a global repr_r for the
currently being operated on repr_r struct. Of course all repr_s's would eventually need to be
malloc'd onto the heap.

This has a few benefits:
- Helper functions can become non-returning, meaning I don't need to set up a stack frame.
Every single helper function right now has a prelude and epilogue for stack handling.
- It would make future assembly optimizations significantly easier as I can then avoid
dealing with the stack!
- Less pointer traversal. Every single helper function does at least one pointer traversal
like return cur->ll_tok. This is a pointer traversal on the heap, a struct access on
a global variable, especially one I throw into a register, would be faster. I don't *really*
know where on the heap or in my RAM the repr_s struct will be allocated to. It's possible
I'd have to switch pages for access, although hopefully GCC and Linux aren't that dumb.

Of course this solution would be very ugly, and cutting out maybe 4-6 instructions per helper function
is a miscalculation of Amdahl's law, but I think its worth considering.
*********************************************************************************************************/


#endif
