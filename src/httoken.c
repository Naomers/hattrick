#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "httoken.h"

//TODO:
// Currently literal number tokenization only works for 2+ ditit lits
// like 10 or 09
// single digit literals like 1 or 0 still get tokenized as idents
// I believe I know what this is, and it's an easy fix
// but a relatively low priority.

enum tokenType_e singleCharReturn(char c){
	switch(c){
		case '+': return tok_op_plus;
		case '-': return tok_op_minus;
		case '*': return tok_op_mult;
		case '/': return tok_op_div;
		case '=': return tok_op_assignment;
		case '@': return tok_op_atSign;
		case '.': return tok_op_dot;
		case '>': return tok_op_greater;
		case '<': return tok_op_less;
		case '\'': return tok_op_squote;
		case '\"': return tok_op_dquote;
		case ')': return tok_op_rpar;
		case '(': return tok_op_lpar;
		case '[': return tok_op_lbrack;
		case ']': return tok_op_rbrack;
		case '{': return tok_op_lbrace;
		case '}': return tok_op_rbrace;
		case ';': return tok_comment;
		case ',': return tok_op_comma;
	}
	return -1;
}

char *substring(const char *s, int ws, int c){
	if (ws < 0 || c < 0 || ws >= c) return NULL;
	int len = c - ws;
	char *sub = malloc(len + 1);
	if(!sub) return NULL;
	memcpy(sub, s + ws, len);
	sub[len] = '\0';
	return sub;
}//TODO: this could be optimized, not necessarily because it's inherently flawed but because it's called a lot
 // A memory buffer to avoid calling malloc so fucking much would be a great idea, but it'd be like picking up
 // a house of cards to put it on top of a basketball, at least in this current messy state of the codebase.
 // According to callgrind 3% of the entire program's execution time is on substring.
 // We could also try to avoid using substring so much, but the operation is currently how the tokenization function works

int peekFor(const char *s, int ws, char peek){
	const char *found = strchr(s + ws, peek);
	return found ? (int)(found - s) : -1;
}

int peekForAny(const char *s, int ws, const char *peeks){
	// as opposed to peekFor which searches for one char
	// this func will search for multiple characters
	int earliest = -1;
	for(const char *p = peeks; *p; ++p){
		const char *f = strchr(s + ws, *p);
		//TODO: test strchr with a duff'd loop instead of the call to the function. 
		// originally this actually used a for loop I wrote and strchr was ever so slightly faster
		// so test a few more things to get the function a little faster
		if(f){
			int indx = (int)(f - s);
			if(earliest == - 1 || indx < earliest){
				earliest = indx;
			}
		}
	}
	return earliest;
} //TODO: this could probbo be optimized with some inline assembly
  // Currently 41 instructions (not including however many strchr is)
  // 41 instructions doesn't sound bad but in a larger context its ran many many times
  // According to Callgrind this function is 2% of the entire runtime of the program

lexerNode_t *appendNode(lexerNode_t *head, lexerNode_t *new){
	if(head == NULL){
		head = new;
		return new;
	}else {
		lexerNode_t *current = head;
		while(current->ll_next != NULL){
			current = current->ll_next;
		}
		current->ll_next = new;
		return head;
	}
}

int isStringNumeric(const char *s){
	int isNumeric = 1;
	while(*s){
		if(!isdigit(*s)){
			isNumeric = 0;
			break;
		}
		*s++;
	}
	return isNumeric;
}

enum tokenType_e getKeywordType(const char *str) {
	int length = strlen(str);
	if (strncmp(str, "if", length) == 0 && length == 2) return tok_kw_if;
	if (strncmp(str, "true", length) == 0 && length == 4) return tok_kw_true;
	if (strncmp(str, "false", length) == 0 && length == 5) return tok_kw_false;
	if (strncmp(str, "null", length) == 0 && length == 4) return tok_kw_null;
	if (strncmp(str, "while", length) == 0 && length == 5) return tok_kw_while;
	if (strncmp(str, "for", length) == 0 && length == 3) return tok_kw_for;
	if (strncmp(str, "return", length) == 0 && length == 6) return tok_kw_return;
	if(isStringNumeric(str)){
		return tok_literal_num;
	}
	return tok_identifier;
}

lexerNode_t *httokenize(const char *s){
	if(globalPool == NULL){
		globalPool = tpPoolInit();
	}

	if(s == NULL) return NULL;
	const char *sep = " {}[]@;=+-/*().,'\"";
	int ws = 0;
	lexerNode_t *head = NULL;
	/* TODO:
	   Check to see if using a tail pointer to prevent walking the list everytime is faster
	   Most lexed stmts will be rather short, maybe 30 tokens at max, so I'll concern myself
	   with that optimization later! */
	while(s[ws] != '\0'){
		int c = peekForAny(s, ws, sep);
		// if c == -1 that means no more seps are found
		// this is the finalization
		if(c == -1){
			if(strlen(s + ws) > 0){
				char *sub = substring(s, ws, strlen(s));
				head = appendNode(head, createNode(sub, globalPool));
				free(sub);
			}
			break;
		}
		// if there are characters before the sep, tokenize them
		if(c > ws){
			char *sub = substring(s, ws, c);
			head = appendNode(head, createNode(sub, globalPool));
			free(sub);
		}
		//tokenize the sep itself
		if(s[c] != ' '){ //whitespaces can be ignored
			char sepStr[2] = { s[c], '\0' };
			head = appendNode(head, createNode(sepStr, globalPool));
			if(s[c] == ';'){
				return head;
			}
		}
		ws = c + 1;
	}
	return head;
}

void debugWalk(lexerNode_t * head){
	lexerNode_t *cur = head;
	while(cur != NULL){
		printf("%d ", cur->ll_tok->tokType);
		cur = cur->ll_next;
	}
}

void determineTokenTypes(lexerNode_t *head){
	lexerNode_t *cur = head; //more fucking pointer walking, fucking great
	int insideQuotesHack = 0; //stupid bad hack
	while(cur != NULL){
		token_t *tok = cur->ll_tok;
		if(tok == NULL){
			//undefined behavior I got no fucking clue what to do here
			printf("I AM CONFUSED PLEASE HELP!!!!!!!!\n");
			return;
		}
		int len = strlen(tok->tokStr);
		tok->tokStrLen = len;
		if(len == 1){
			tok->tokType = singleCharReturn(tok->tokStr[0]);
			if(tok->tokType == -1){
				tok->tokType = tok_identifier;
			}
		}else {
			tok->tokType = getKeywordType(tok->tokStr);
			//TODO: identify literals like 1234 or "foo" 
		}
		if(insideQuotesHack){
			tok->tokType = tok_literal_str;
			int isQuote = singleCharReturn(tok->tokStr[0]);
			if(isQuote == tok_op_squote || isQuote == tok_op_dquote){
				tok->tokType = isQuote;
			}
		} // This hack is lazy and awful. Fix it retard

		if(tok->tokType == tok_op_squote || tok->tokType == tok_op_dquote){
			insideQuotesHack = 1;
		}

		switch(tok->tokType){
			case tok_identifier:
			case tok_literal_str:
			case tok_literal_num:
				break;
			default:
				free(tok->tokStr);
				tok->tokStr = NULL;
				break;
		}
		cur = cur->ll_next;
	}
}
