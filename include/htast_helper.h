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




#endif
