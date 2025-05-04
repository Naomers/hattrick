#ifndef HTAST_MEM_H
#define HTAST_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include "httoken.h"
#include "htast.h"
#include "httoken_mem.h"

void freeRep(struct repr_s *r);
void freeStmt(struct stmt_s *s);

#endif
