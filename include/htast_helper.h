#ifndef HTAST_HELPER_H
#define HTAST_HELPER_H

//#define ADVANCE(cur) do { if ((cur)->ll_next) (cur) = (cur)->ll_next; } while(0)
//#define ADVANCE(cur) if ((cur)->ll_next) (cur) = (cur)->ll_next;
void ADVANCE(lexerNode_t **cur) {
    if ((*cur)->ll_next) {
        *cur = (*cur)->ll_next;
	return;
    }
    *cur = NULL;
}


/*
#define ADVANCE(cur) \
    do { \
        if ((cur) && (*(cur)) && (*(cur))->ll_next) { \
            *(cur) = (*(cur))->ll_next; \
        } else { \
            *(cur) = NULL; \
        } \
    } while (0)

*/

enum tokenType_e tkType(lexerNode_t *cur){
	if(cur->ll_tok){
		return cur->ll_tok->tokType;
	}
	return tok_INVALID;
}

lexerNode_t *next(lexerNode_t *cur){
	if(cur->ll_next){
		return cur->ll_next;
	}
	return NULL;
}

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
} //this code will be signficantly better when tkType gets macro-ized too!

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
