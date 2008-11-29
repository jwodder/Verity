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
#include <string.h>
#include "veriprint.h"
#include "veritypes.h"
#include "verity.tab.h"

/* #define OLD_XOR_SYM */

void printDocTop(void) {
 if (flags.tblType == latexTbl)
  puts("\\documentclass{article}\n\\begin{document}\n\\begin{center}");
 else if (flags.tblType == psTbl) {
  puts("%!PS-Adobe-3.0\n"
   "/fontSize 12 def\n"
   "/varFont /Times-Italic findfont fontSize scalefont def\n"
   "/truthFont /Times-Roman findfont fontSize scalefont def\n"
   "truthFont setfont /em (M) stringwidth pop def\n"
   "/TPad em (T) stringwidth pop sub 2 div def\n"
   "/FPad em (F) stringwidth pop sub 2 div def\n"
   "/barPad em 4 div def\n"
   /* barPad = padding on each side of table bars, including half the width of
    * the bars */
   "/symFont /Symbol findfont fontSize scalefont def\n"
   "72 720 fontSize add moveto\n"
  );
 }
}

void printDocEnd(void) {
 if (flags.tblType == latexTbl) puts("\\end{center}\n\\end{document}");
 else if (flags.tblType == texTbl) puts("\\bye");
 else if (flags.tblType == psTbl) puts("stroke showpage");
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
  case psTbl: printPSTbl(vars, varno); break;
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
#ifdef OLD_XOR_SYM
  case XOR: binOp = " \\dot{\\vee} "; break;
#else
  case XOR: binOp = " \\oplus "; break;
#endif
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
#ifdef OLD_XOR_SYM
  case XOR: binOp = "x"; break;
#else
  case XOR: binOp = "+"; break;  /* Consider this */
#endif
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

void printPSTbl(symbol** vars, int varno) {
 puts("/tableTop currentpoint exch pop fontSize sub def\n"
  "72 tableTop fontSize sub moveto varFont setfont");
 printf("/stmntWidths %d array def\n", stmntQty);
 int i;
 for (i=0; i<varno; i++) {
  puts("barPad 0 rmoveto");
  printf("(%c) dup stringwidth pop em exch sub 2 div dup 0 rmoveto exch show\n",
   vars[i]->c);
  puts("barPad add 0 rmoveto");
 }
 i=0;
 for (expr* ex = statements; ex != NULL; ex = ex->next) {
  puts("barPad 0 rmoveto /exprStart currentpoint pop def");
  printPSExp(ex);
  printf("stmntWidths %d currentpoint pop exprStart sub put\n", i++);
  puts("barPad 0 rmoveto");
 }
 puts("0 barPad neg rmoveto 72 currentpoint exch pop lineto\n"
  "0 barPad neg rmoveto truthFont setfont");
 do {
  puts("72 currentpoint exch pop fontSize sub moveto");
  for (i=0; i<varno; i++) {
   char truth = vars[i]->truth ? 'T' : 'F';
   printf("%cPad barPad add dup 0 rmoveto (%c) show 0 rmoveto\n", truth, truth);
  }
  i=0;
  for (expr* ex = statements; ex != NULL; ex = ex->next) {
   puts("barPad 0 rmoveto");
   char truth = evalExpr(ex) ? 'T' : 'F';
   printf("stmntWidths %d get (%c) stringwidth pop sub 2 div dup 0 rmoveto (%c) show barPad add 0 rmoveto\n", i++, truth, truth);
  }
  for (i=varno-1; i>=0; i--) if (!(vars[i]->truth = !(vars[i]->truth))) break;
 } while (i >= 0);
 puts("/tableBottom currentpoint exch pop def");
 printf("1 1 %d {\n"
  " em barPad 2 mul add mul 72 add dup tableBottom moveto tableTop lineto\n"
  "} for\n", varno);
 printf("0 1 %d 2 sub {\n"
  " stmntWidths exch get barPad 2 mul add 0 rmoveto\n"
  " 0 tableBottom tableTop sub rlineto\n"
  " 0 tableTop tableBottom sub rmoveto\n"
  "} for\n", stmntQty);
 puts("0 tableBottom tableTop sub rmoveto");
}

void printPSExp(expr* ex) {
 if (!ex) return; /* Do something else? */
 if (ex->paren) puts("truthFont setfont (\\() show varFont setfont");
 char* binOp = NULL;
 switch (ex->oper) {
  case 0: printf("(%c) show\n", ex->sym->c); break;
  case NOT:
   puts("symFont setfont <D8> show varFont setfont");
   printPSExp(ex->args[0]);
   break;
  case AND: binOp = "<D9>"; break;
  case OR: binOp = "<DA>"; break;
  case XOR:
#ifdef OLD_XOR_SYM
   printPSExp(ex->args[0]);
   puts("symFont setfont\n"
    "<DA> dup show stringwidth pop 2 div dup neg 0 rmoveto\n"
    "truthFont setfont <C7> dup stringwidth pop -2 div dup 0 rmoveto\n"
    "exch show add 0 rmoveto varFont setfont");
   printPSExp(ex->args[1]);
#else
   binOp = "<C5>";
#endif
   break;
  case THEN: binOp = "<AE>"; break;
  case EQ: binOp = "<AB>"; break;
  case ':':
   printf("(%c) show\n", ex->sym->c);
   puts("truthFont setfont ( : ) show varFont setfont");
   printPSExp(ex->args[0]);
   break;
  default: return; /* And/or do something else? */
 }
 if (binOp != NULL) {
  printPSExp(ex->args[0]);
  printf("symFont setfont %s show varFont setfont\n", binOp);
  printPSExp(ex->args[1]);
 }
 if (ex->paren) puts("truthFont setfont (\\)) show varFont setfont");
}
