#ifndef HTAST_PRINT_H
#define HTAST_PRINT_H


void printreplit(struct repr_s *r);
void printrepident(struct repr_s *r);
void printOperation(struct repr_s *r);
void printrepCall(struct repr_s *r);

void printReturn(struct stmt_s *r);
void printAssignment(struct stmt_s *a);
void printDef(struct stmt_s *d);
void printRep(struct repr_s *r);


#endif
