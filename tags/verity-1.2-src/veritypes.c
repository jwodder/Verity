/* Verity - generates truth tables of logical statements
   Copyright (C) 2007 John T. Wodder II

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

#include <stdio.h>
#include <stdlib.h>
#include "veritypes.h"
#include "verity.tab.h"

linkedList *symTbl, *statements;

void pushItem(linkedList* list, void* value) {
 linkedItem* item = malloc(sizeof(linkedItem));
 checkMem(item);
 item->val = value;
 item->next = NULL;
 if (list->qty++) list->last = list->last->next = item;
 else list->first = list->last = item;
}

void initLists(void) {
 symTbl = malloc(sizeof(linkedList));
 checkMem(symTbl);
 symTbl->qty = 0;
 symTbl->first = symTbl->last = NULL;
 statements = malloc(sizeof(linkedList));
 checkMem(statements);
 statements->qty = 0;
 statements->first = statements->last = NULL;
}

symbol* getSym(char c) {
 foreach(symTbl, s) if (((symbol*)s->val)->c == c) return s->val;
 symbol* sym = newSym(c);
 pushItem(symTbl, sym);
 return sym;
}

symbol* newSym(char c) {
 symbol* sym = malloc(sizeof(symbol));
 checkMem(sym);
 sym->c = c;
 sym->truth = 1; /* Right? */
 return sym;
}

expr* symExpr(symbol* sym) {
 if (!sym) return NULL; expr* ex = malloc(sizeof(expr)); checkMem(ex);
 ex->oper = ex->paren = 0; ex->sym = sym; return ex;
}

expr* notExpr(expr* ex) {
 if (!ex) return NULL;
 expr* negation = malloc(sizeof(expr) + sizeof(expr*)); checkMem(negation);
 negation->oper = NOT; negation->sym = NULL; negation->args[0] = ex;
 negation->paren = 0;
 return negation;
}

expr* opExpr(int op, expr* left, expr* right) {
 expr* operation = malloc(sizeof(expr) + 2 * sizeof(expr*));
 checkMem(operation);
 operation->oper = op; operation->sym = NULL; operation->paren = 0;
 operation->args[0] = left; operation->args[1] = right;
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
 assignment->sym = sym;
 assignment->paren = 0;
 sym->truth = 0;
 assignment->args[0] = ex;
 return assignment;
}

void clearLists(void) {
 linkedItem* item = symTbl->first;
 symTbl->qty = 0; symTbl->first = symTbl->last = NULL;
 while (item) {
  free(item->val); linkedItem* next = item->next; free(item); item = next;
 }
 item = statements->first;
 statements->qty = 0; statements->first = statements->last = NULL;
 while (item) {
  freeExpr(item->val); linkedItem* next = item->next; free(item); item = next;
 }
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
