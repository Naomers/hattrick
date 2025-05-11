#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "httoken.h"

char *substring(const char *s, int ws, int c){
	if (ws < 0 || c < 0 || ws >= c) return NULL;
	int len = c - ws;
	char *sub = malloc(len + 1);
	if(!sub) return NULL;
	memcpy(sub, s + ws, len);
	sub[len] = '\0';
	return sub;
}

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
		if(f){
			int indx = (int)(f - s);
			if(earliest == - 1 || indx < earliest){
				earliest = indx;
			}
		}
	}
	return earliest;
}
