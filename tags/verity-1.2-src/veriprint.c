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
#include "veriprint.h"
#include "veritypes.h"
#include "verity.tab.h"

extern struct {
 enum {txtTbl=0, latexTbl, texTbl} tblType;
 _Bool eval, standalone;
} flags;

symbol** vars;
int varno;

void printTbl(FILE* out) {
 varno = 0;
 vars = calloc(symTbl->qty, sizeof(symbol*));
 checkMem(vars);
 foreach(symTbl, s) if (((symbol*)s->val)->truth) vars[varno++] = s->val;
 if (statements->qty)
  switch (flags.tblType) {
   case latexTbl: printLaTeXTbl(out); break;
   case texTbl: printTeXTbl(out); break;
   case txtTbl: printTxtTbl(out); break;
  }
 fputc(10, out);
 free(vars);
}

void printLaTeXTbl(FILE* tex) {
 int i;
 fputs("\\begin{tabular}{", tex);
 for (i=0; i < varno + statements->qty; i++) {
  if (i) fputc('|', tex); fputc('c', tex);
 }
 fputs("}\n", tex);
 for (i=0; i<varno; i++) {
  if (i) fputs(" & ", tex);
  fputc('$', tex); fputc(vars[i]->c, tex); fputc('$', tex);
 }
 foreach(statements, ex) {
  fputs(" & $", tex); printTeXExp(ex->val, tex); fputc('$', tex);
 }
 fputs(" \\\\ \\hline\n", tex);
 do {
  for (i=0; i<varno; i++) {
   if (i) fputs(" & ", tex); fputc(vars[i]->truth ? 'T' : 'F', tex);
  }
  foreach(statements, ex) {
   fputs(" & ", tex); fputc(evalExpr(ex->val) ? 'T' : 'F', tex);
  }
  fputs(" \\\\\n", tex);
  for (i=varno-1; i>-1; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i > -1);
 fputs("\\end{tabular}\n", tex);
}

void printTeXExp(expr* ex, FILE* tex) {
 if (!ex) return; /* Do something else? */
 if (ex->paren) fputc('(', tex);
 switch (ex->oper) {
  case 0: fputc(ex->sym->c, tex); break;
  case NOT: fputs("\\neg ", tex); printTeXExp(ex->args[0], tex); break;
  case AND: 
   printTeXExp(ex->args[0], tex); fputs(" \\wedge ", tex);
   printTeXExp(ex->args[1], tex); break;
  case OR:
   printTeXExp(ex->args[0], tex); fputs(" \\vee ", tex);
   printTeXExp(ex->args[1], tex); break;
  case XOR:
   printTeXExp(ex->args[0], tex); fputs(" \\dot{\\vee} ", tex);
   printTeXExp(ex->args[1], tex); break;
  case THEN:
   printTeXExp(ex->args[0], tex); fputs(" \\rightarrow ", tex);
   printTeXExp(ex->args[1], tex); break;
  case EQ:
   printTeXExp(ex->args[0], tex); fputs(" \\leftrightarrow ", tex);
   printTeXExp(ex->args[1], tex); break;
  case ':':
   fputc(ex->sym->c, tex); fputs("\\>:\\>", tex);
   printTeXExp(ex->args[0], tex); break;
  default: return; /* And/or do something else? */
 }
 if (ex->paren) fputc(')', tex);
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

void printTxtTbl(FILE* txt) {
 int i;
 for (i=0; i<varno; i++) {if (i) fputc('|', txt); fputc(vars[i]->c, txt); }
 int* stmntLen = calloc(statements->qty, sizeof(int));
 checkMem(stmntLen);
 i=0;
 foreach(statements, ex) {
  fputc('|', txt); printTxtExp(ex->val, txt);
  stmntLen[i++] = exprLength(ex->val);
  /* Should printTxtExp() return the length instead? */
 }
 fputc(10, txt);
 for (i=0; i<varno; i++) {if (i) fputc('|', txt); fputc('-', txt); }
 for (i=0; i < statements->qty; i++) {
  fputc('|', txt); for (int j=0; j<stmntLen[i]; j++) fputc('-', txt);
 }
 fputc(10, txt);
 do {
  for (i=0; i<varno; i++) {
   if (i) fputc('|', txt); fputc(vars[i]->truth ? 'T' : 'F', txt);
  }
  i=0;
  foreach(statements, ex) {
   fputc('|', txt);
   int j, len = stmntLen[i]/2 - !(stmntLen[i] % 2);
   for (j=0; j<len; j++) fputc(32, txt);
   fputc(evalExpr(ex->val) ? 'T' : 'F', txt);
   len += !(stmntLen[i] % 2);
   if (ex->next) for (j=0; j<len; j++) fputc(32, txt);
   i++;
  }
  fputc(10, txt);
  for (i=varno-1; i>-1; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i > -1);
}

void printTxtExp(expr* ex, FILE* txt) {
 if (!ex) return; /* Do something else? */
 if (ex->paren) fputc('(', txt);
 switch (ex->oper) {
  case 0: fputc(ex->sym->c, txt); break;
  case NOT: fputc('-', txt); printTxtExp(ex->args[0], txt); break;
  case AND:
   printTxtExp(ex->args[0], txt); fputc('^', txt);
   printTxtExp(ex->args[1], txt); break;
  case OR:
   printTxtExp(ex->args[0], txt); fputc('v', txt);
   printTxtExp(ex->args[1], txt); break;
  case XOR:
   printTxtExp(ex->args[0], txt); fputc('x', txt);
   printTxtExp(ex->args[1], txt); break;
  case THEN:
   printTxtExp(ex->args[0], txt); fputs("->", txt);
   printTxtExp(ex->args[1], txt); break;
  case EQ:
   printTxtExp(ex->args[0], txt); fputs("<->", txt);
   printTxtExp(ex->args[1], txt); break;
  case ':':
   fputc(ex->sym->c, txt); fputs(" : ", txt);
   printTxtExp(ex->args[0], txt); break;
  default: return; /* And/or do something else? */
 }
 if (ex->paren) fputc(')', txt);
}

void printDocTop(FILE* out) {
 if (flags.tblType == latexTbl)
  fputs("\\documentclass{article}\n\\begin{document}\n\\begin{center}\n", out);
}

void printDocEnd(FILE* out) {
 if (flags.tblType == latexTbl) fputs("\\end{center}\n\\end{document}\n", out);
 else if (flags.tblType == texTbl) fputs("\\bye\n", out);
}

void printTeXTbl(FILE* tex) {
 int i;
 fputs("$$\\vbox{\\offinterlineskip\\halign{\n\\strut", tex);
 for (i=0; i < varno + statements->qty; i++) {
  if (i) fputs(" & \\vrule", tex); fputs("\\hfil\\ #\\ \\hfil", tex);
 }
 fputs("\\cr\n", tex);
 for (i=0; i<varno; i++) {
  if (i) fputs(" & ", tex);
  fputc('$', tex); fputc(vars[i]->c, tex); fputc('$', tex);
 }
 foreach(statements, ex) {
  fputs(" & $", tex); printTeXExp(ex->val, tex); fputc('$', tex);
 }
 fputs("\\cr\\noalign{\\hrule}\n", tex);
 do {
  for (i=0; i<varno; i++) {
   if (i) fputs(" & ", tex); fputc(vars[i]->truth ? 'T' : 'F', tex);
  }
  foreach(statements, ex) {
   fputs(" & ", tex); fputc(evalExpr(ex->val) ? 'T' : 'F', tex);
  }
  fputs("\\cr\n", tex);
  for (i=varno-1; i>-1; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i > -1);
 fputs("}}$$\n", tex);
}
