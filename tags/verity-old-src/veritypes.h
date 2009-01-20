/* Verity - generates truth tables of logical statements
   Copyright (C) 2007 John T. Wodder II

   This file is part of Verity.

   Verity is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Verity is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Verity; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#ifndef VERITYPES_H
#define VERITYPES_H
#include <stdio.h>

#ifndef checkMem
#define checkMem(p) if (!p) {fprintf(stderr, "Error: %s, l.%d: could not allocate suffient memory\n", __FILE__, __LINE__); exit(1); }
#endif

typedef struct {char c; _Bool truth; } symbol;
typedef struct expr {int oper, paren; symbol* sym; struct expr* args[]; } expr;
typedef struct linkedItem {void* val; struct linkedItem* next; } linkedItem;
typedef struct {int qty; linkedItem *first, *last; } linkedList;
#define foreach(ll, it) for (linkedItem* it = ll ? ll->first : NULL; it != \
 NULL; it = it->next)

extern linkedList *symTbl, *statements;

void pushItem(linkedList* list, void* value);
void initLists(void);
symbol* getSym(char c);
symbol* newSym(char c);
expr* symExpr(symbol* sym);
expr* notExpr(expr* ex);
expr* opExpr(int op, expr* left, expr* right);
expr* parenExpr(expr* ex);
_Bool evalExpr(expr* ex);
expr* colonExpr(symbol* sym, expr* ex);
void clearLists(void);
void freeExpr(expr* ex);
#endif
