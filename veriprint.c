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
#include "veriprint.h"
#include "veritypes.h"
#include "verity.tab.h"

extern struct {
 enum {txtTbl=0, latexTbl, texTbl} tblType;
 _Bool eval, standalone : 1;
} flags;

symbol** vars;
int varno;

void printTbl(void) {
 varno = 0;
 vars = calloc(symQty, sizeof(symbol*));
 checkMem(vars);
 for (symbol* s = symTbl; s != NULL; s = s->next)
  if (s->truth) vars[varno++] = s;
 switch (flags.tblType) {
  case latexTbl: printLaTeXTbl(); break;
  case texTbl: printTeXTbl(); break;
  case txtTbl: printTxtTbl(); break;
 }
 putchar('\n');
 free(vars);
}

void printLaTeXTbl(void) {
 int i;
 fputs("\\begin{tabular}{", stdout);
 for (i=0; i < varno + stmntQty; i++) {
  if (i) putchar('|'); putchar('c');
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

void printTeXExp(expr* ex) {
 if (!ex) return; /* Do something else? */
 if (ex->paren) putchar('(');
 switch (ex->oper) {
  case 0: putchar(ex->sym->c); break;
  case NOT: fputs("\\neg ", stdout); printTeXExp(ex->args[0]); break;
  case AND: 
   printTeXExp(ex->args[0]); fputs(" \\wedge ", stdout);
   printTeXExp(ex->args[1]); break;
  case OR:
   printTeXExp(ex->args[0]); fputs(" \\vee ", stdout);
   printTeXExp(ex->args[1]); break;
  case XOR:
   printTeXExp(ex->args[0]); fputs(" \\dot{\\vee} ", stdout);
   printTeXExp(ex->args[1]); break;
  case THEN:
   printTeXExp(ex->args[0]); fputs(" \\rightarrow ", stdout);
   printTeXExp(ex->args[1]); break;
  case EQ:
   printTeXExp(ex->args[0]); fputs(" \\leftrightarrow ", stdout);
   printTeXExp(ex->args[1]); break;
  case ':':
   putchar(ex->sym->c); fputs("\\>:\\>", stdout);
   printTeXExp(ex->args[0]); break;
  default: return; /* And/or do something else? */
 }
 if (ex->paren) putchar(')');
}

int exprLength(expr* ex) { /* Probably needs improvement */
 if (!ex) return 0;
 switch (ex->oper) {
  case 0: return 1 + 2 * ex->paren;
  case NOT: return 1 + exprLength(ex->args[0]) + 2 * ex->paren;
  case AND: case OR: case XOR:
   return 1 + exprLength(ex->args[0]) + exprLength(ex->args[1]) + 2 * ex->paren;
  case THEN:
   return 2 + exprLength(ex->args[0]) + exprLength(ex->args[1]) + 2 * ex->paren;
  case EQ:
   return 3 + exprLength(ex->args[0]) + exprLength(ex->args[1]) + 2 * ex->paren;
  case ':': return 4 + exprLength(ex->args[0]);
  default: return 0;
 }
}

void printTxtTbl(void) {
 int i;
 for (i=0; i<varno; i++) {if (i) putchar('|'); putchar(vars[i]->c); }
 int* stmntLen = calloc(stmntQty, sizeof(int));
 checkMem(stmntLen);
 i=0;
 for (expr* ex = statements; ex != NULL; ex = ex->next) {
  putchar('|');
  printTxtExp(ex);
  stmntLen[i++] = exprLength(ex);
  /* Should printTxtExp() return the length instead? */
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

void printTxtExp(expr* ex) {
 if (!ex) return; /* Do something else? */
 if (ex->paren) putchar('(');
 switch (ex->oper) {
  case 0: putchar(ex->sym->c); break;
  case NOT: putchar('-'); printTxtExp(ex->args[0]); break;
  case AND:
   printTxtExp(ex->args[0]); putchar('^'); printTxtExp(ex->args[1]); break;
  case OR:
   printTxtExp(ex->args[0]); putchar('v'); printTxtExp(ex->args[1]); break;
  case XOR:
   printTxtExp(ex->args[0]); putchar('x'); printTxtExp(ex->args[1]); break;
  case THEN:
   printTxtExp(ex->args[0]); fputs("->", stdout);
   printTxtExp(ex->args[1]); break;
  case EQ:
   printTxtExp(ex->args[0]); fputs("<->", stdout);
   printTxtExp(ex->args[1]); break;
  case ':':
   putchar(ex->sym->c); fputs(" : ", stdout); printTxtExp(ex->args[0]); break;
  default: return; /* And/or do something else? */
 }
 if (ex->paren) putchar(')');
}

void printDocTop(void) {
 if (flags.tblType == latexTbl)
  puts("\\documentclass{article}\n\\begin{document}\n\\begin{center}");
}

void printDocEnd(void) {
 if (flags.tblType == latexTbl) puts("\\end{center}\n\\end{document}");
 else if (flags.tblType == texTbl) puts("\\bye");
}

void printTeXTbl(void) {
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
