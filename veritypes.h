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

#ifndef VERITYPES_H
#define VERITYPES_H
#include <stdio.h>
#include <stdlib.h>

#define checkMem(p)  if (!(p)) {perror("verity"); exit(1); }

typedef struct symbol {
 char c;
 _Bool truth;
 int refQty;
 struct symbol* next;
} symbol;

typedef struct expr {
 int oper, paren;
 struct expr* next;
 symbol* sym;
 struct expr* args[];
} expr;

struct {
 enum {txtTbl=0, wideTbl, utfTbl, latexTbl, texTbl, psTbl} tblType;
 _Bool eval : 1, standalone : 1;
} flags;

extern symbol* symTbl;
extern expr* statements;
extern int symQty, stmntQty;

void addStmnt(expr* ex);
symbol* getSym(char c);
expr* symExpr(symbol* sym);
expr* notExpr(expr* ex);
expr* opExpr(int op, expr* left, expr* right);
expr* parenExpr(expr* ex);
_Bool evalExpr(expr* ex);
expr* colonExpr(symbol* sym, expr* ex);
void clearLists(void);
void freeExpr(expr* ex);
#endif
