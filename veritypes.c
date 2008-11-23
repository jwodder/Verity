/* Verity - generates truth tables of logical statements
   Copyright (C) 2007, 2008 John T. Wodder II

   This file is part of Verity.

   Verity is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Verity is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Verity.  If not, see <http://www.gnu.org/licenses/>. */

/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include "veritypes.h"
#include "verity.tab.h"

symbol* symTbl = NULL;
expr* statements = NULL;
int symQty = 0, stmntQty = 0;

void addStmnt(expr* ex) {
 if (statements == NULL) {
  statements = ex;
 } else {
  expr* prev = statements;
  while (prev->next != NULL) prev = prev->next;
  prev->next = ex;
 }
 stmntQty++;
}

symbol* getSym(char c) {
 symbol *s = symTbl, *prev = NULL;
 for (; s != NULL; prev = s, s = s->next)
  if (s->c == c) return s;
 symbol* sym = malloc(sizeof(symbol));
 checkMem(sym);
 sym->c = c;
 sym->truth = 1;
 sym->next = NULL;
 if (prev == NULL) symTbl = sym;
 else prev->next = sym;
 symQty++;
 return sym;
}

expr* symExpr(symbol* sym) {
 if (!sym) return NULL;
 expr* ex = malloc(sizeof(expr));
 checkMem(ex);
 ex->oper = ex->paren = 0;
 ex->next = NULL;
 ex->sym = sym;
 return ex;
}

expr* notExpr(expr* ex) {
 if (!ex) return NULL;
 expr* negation = malloc(sizeof(expr) + sizeof(expr*));
 checkMem(negation);
 negation->oper = NOT;
 negation->next = NULL;
 negation->sym = NULL;
 negation->args[0] = ex;
 negation->paren = 0;
 return negation;
}

expr* opExpr(int op, expr* left, expr* right) {
 expr* operation = malloc(sizeof(expr) + 2 * sizeof(expr*));
 checkMem(operation);
 operation->oper = op;
 operation->next = NULL;
 operation->sym = NULL;
 operation->paren = 0;
 operation->args[0] = left;
 operation->args[1] = right;
 return operation;
}

expr* parenExpr(expr* ex) {if (!ex) return NULL; ex->paren = 1; return ex; }

_Bool evalExpr(expr* ex) {
 if (!ex) return 0;
 switch (ex->oper) {
  case 0: return ex->sym->truth;
  case NOT: return !(evalExpr(ex->args[0]));
  case AND: return evalExpr(ex->args[0]) && evalExpr(ex->args[1]);
  case OR: return evalExpr(ex->args[0]) || evalExpr(ex->args[1]);
  case XOR: {
   _Bool a = evalExpr(ex->args[0]), b = evalExpr(ex->args[1]);
   return (a || b) && !(a && b);
  }
  case THEN: return !evalExpr(ex->args[0]) || evalExpr(ex->args[1]);
  case EQ: return evalExpr(ex->args[0]) == evalExpr(ex->args[1]);
  case ':': return ex->sym->truth = evalExpr(ex->args[0]);
  default: return 0; /* And/or do something else? */
 }
}

expr* colonExpr(symbol* sym, expr* ex) {
 expr* assignment = malloc(sizeof(expr) + sizeof(expr*));
 checkMem(assignment);
 assignment->oper = ':';
 assignment->next = NULL;
 assignment->sym = sym;
 assignment->paren = 0;
 sym->truth = 0;
 assignment->args[0] = ex;
 return assignment;
}

void clearLists(void) {
 for (symbol* s = symTbl; s != NULL; ) {
  symbol* next = s->next;
  free(s);
  s = next;
 }
 symTbl = NULL;
 symQty = 0;
 for (expr* ex = statements; ex != NULL; ) {
  expr* next = ex->next;
  freeExpr(ex);
  ex = next;
 }
 statements = NULL;
 stmntQty = 0;
}

void freeExpr(expr* ex) {
 if (!ex) return;
 int arity = 2;
 switch (ex->oper) {
  case 0: /*free(ex->sym);*/ break;
  case ':': /*free(ex->sym);*/
  case NOT: arity = 1;
  default: for (int i=0; i<arity; i++) freeExpr(ex->args[i]);
 }
 free(ex);
}
