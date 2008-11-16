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

/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veriprint.h"
#include "veritypes.h"
#include "verity.tab.h"

extern struct {
 enum {txtTbl=0, latexTbl, texTbl} tblType;
 _Bool eval, standalone : 1;
} flags;

void printDocTop(void) {
 if (flags.tblType == latexTbl)
  puts("\\documentclass{article}\n\\begin{document}\n\\begin{center}");
}

void printDocEnd(void) {
 if (flags.tblType == latexTbl) puts("\\end{center}\n\\end{document}");
 else if (flags.tblType == texTbl) puts("\\bye");
}

void printTbl(void) {
 int varno = 0;
 symbol** vars = calloc(symQty, sizeof(symbol*));
 checkMem(vars);
 for (symbol* s = symTbl; s != NULL; s = s->next)
  if (s->truth) vars[varno++] = s;
 switch (flags.tblType) {
  case latexTbl: printLaTeXTbl(vars, varno); break;
  case texTbl: printTeXTbl(vars, varno); break;
  case txtTbl: printTxtTbl(vars, varno); break;
 }
 putchar('\n');
 free(vars);
}

void printLaTeXTbl(symbol** vars, int varno) {
 int i;
 fputs("\\begin{tabular}{", stdout);
 for (i=0; i < varno + stmntQty; i++) {
  if (i) putchar('|');
  putchar('c');
 }
 fputs("}\n", stdout);
 for (i=0; i<varno; i++) {
  if (i) fputs(" & ", stdout);
  putchar('$'); putchar(vars[i]->c); putchar('$');
 }
 for (expr* ex = statements; ex != NULL; ex = ex->next) {
  fputs(" & $", stdout);
  printTeXExp(ex);
  putchar('$');
 }
 fputs(" \\\\ \\hline\n", stdout);
 do {
  for (i=0; i<varno; i++) {
   if (i) fputs(" & ", stdout);
   putchar(vars[i]->truth ? 'T' : 'F');
  }
  for (expr* ex = statements; ex != NULL; ex = ex->next) {
   fputs(" & ", stdout);
   putchar(evalExpr(ex) ? 'T' : 'F');
  }
  fputs(" \\\\\n", stdout);
  for (i=varno-1; i>=0; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i >= 0);
 fputs("\\end{tabular}\n", stdout);
}

void printTeXTbl(symbol** vars, int varno) {
 int i;
 fputs("$$\\vbox{\\offinterlineskip\\halign{\n\\strut", stdout);
 for (i=0; i < varno + stmntQty; i++) {
  if (i) fputs(" & \\vrule", stdout);
  fputs("\\hfil\\ #\\ \\hfil", stdout);
 }
 fputs("\\cr\n", stdout);
 for (i=0; i<varno; i++) {
  if (i) fputs(" & ", stdout);
  putchar('$');
  putchar(vars[i]->c);
  putchar('$');
 }
 for (expr* ex = statements; ex != NULL; ex = ex->next) {
  fputs(" & $", stdout);
  printTeXExp(ex);
  putchar('$');
 }
 fputs("\\cr\\noalign{\\hrule}\n", stdout);
 do {
  for (i=0; i<varno; i++) {
   if (i) fputs(" & ", stdout);
   putchar(vars[i]->truth ? 'T' : 'F');
  }
  for (expr* ex = statements; ex != NULL; ex = ex->next) {
   fputs(" & ", stdout);
   putchar(evalExpr(ex) ? 'T' : 'F');
  }
  fputs("\\cr\n", stdout);
  for (i=varno-1; i>=0; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i >= 0);
 fputs("}}$$\n", stdout);
}

void printTeXExp(expr* ex) {
 if (!ex) return; /* Do something else? */
 if (ex->paren) putchar('(');
 char* binOp = NULL;
 switch (ex->oper) {
  case 0: putchar(ex->sym->c); break;
  case NOT: fputs("\\neg ", stdout); printTeXExp(ex->args[0]); break;
  case AND: binOp = " \\wedge "; break;
  case OR: binOp = " \\vee "; break;
  case XOR: binOp = " \\dot{\\vee} "; break;
  case THEN: binOp = " \\rightarrow "; break;
  case EQ: binOp = " \\leftrightarrow "; break;
  case ':':
   putchar(ex->sym->c);
   fputs("\\>:\\>", stdout);
   printTeXExp(ex->args[0]);
   break;
  default: return; /* And/or do something else? */
 }
 if (binOp != NULL) {
  printTeXExp(ex->args[0]);
  fputs(binOp, stdout);
  printTeXExp(ex->args[1]);
 }
 if (ex->paren) putchar(')');
}

void printTxtTbl(symbol** vars, int varno) {
 int i;
 for (i=0; i<varno; i++) {if (i) putchar('|'); putchar(vars[i]->c); }
 int* stmntLen = calloc(stmntQty, sizeof(int));
 checkMem(stmntLen);
 i=0;
 for (expr* ex = statements; ex != NULL; ex = ex->next) {
  putchar('|');
  stmntLen[i++] = printTxtExp(ex);
 }
 putchar('\n');
 for (i=0; i<varno; i++) {if (i) putchar('|'); putchar('-'); }
 for (i=0; i<stmntQty; i++) {
  putchar('|');
  for (int j=0; j<stmntLen[i]; j++) putchar('-');
 }
 putchar('\n');
 do {
  for (i=0; i<varno; i++) {
   if (i) putchar('|');
   putchar(vars[i]->truth ? 'T' : 'F');
  }
  i=0;
  for (expr* ex = statements; ex != NULL; ex = ex->next) {
   putchar('|');
   int j, len = stmntLen[i]/2 - !(stmntLen[i] % 2);
   for (j=0; j<len; j++) putchar(' ');
   putchar(evalExpr(ex) ? 'T' : 'F');
   len += !(stmntLen[i] % 2);
   if (ex->next) for (j=0; j<len; j++) putchar(' ');
   i++;
  }
  putchar('\n');
  for (i=varno-1; i>=0; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i >= 0);
}

int printTxtExp(expr* ex) {
 if (!ex) return 0; /* Do something else? */
 int len = ex->paren ? 2 : 0;
 if (ex->paren) putchar('(');
 char* binOp = NULL;
 switch (ex->oper) {
  case 0: putchar(ex->sym->c); len++; break;
  case NOT: putchar('-'); len += 1 + printTxtExp(ex->args[0]); break;
  case AND: binOp = "^"; break;
  case OR: binOp = "v"; break;
  case XOR: binOp = "x"; break;
  case THEN: binOp = "->"; break;
  case EQ: binOp = "<->"; break;
  case ':':
   putchar(ex->sym->c);
   fputs(" : ", stdout);
   len += 4 + printTxtExp(ex->args[0]);
   break;
  default: return len; /* And/or do something else? */
 }
 if (binOp != NULL) {
  len += printTxtExp(ex->args[0]);
  len += strlen(binOp);
  fputs(binOp, stdout);
  len += printTxtExp(ex->args[1]);
 }
 if (ex->paren) putchar(')');
 return len;
}
