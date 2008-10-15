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

%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "veritypes.h"
#include "veriprint.h"
void yyerror(char*);
int yylex(void);
extern FILE* yyin;

struct {
 enum {txtTbl=0, latexTbl, texTbl} tblType;
 _Bool eval, standalone : 1;
} flags = {0};
%}
%union {symbol* sym; expr* stmnt; }
%token <sym> SYM
%type <stmnt> expr statement
%nonassoc '{' '}'
%nonassoc ':' EOL
%left EQ
%left THEN
%left AND OR XOR
%nonassoc NOT
%nonassoc '(' ')'
%%
file: exprSet {printTbl(); if (flags.standalone) printDocEnd(); }
	| blocks {if (flags.standalone) printDocEnd(); } | ;

blocks: blocks '{' exprSet '}'  {printTbl(); clearLists(); }
	| '{' exprSet '}'  {printTbl(); clearLists(); }
	| blocks EOL | EOL blocks ;

exprSet: exprSet EOL statement  {pushItem(statements, $3); }
	| statement  {pushItem(statements, $1); }
	| EOL statement  {pushItem(statements, $2); }
	| exprSet EOL | EOL ;

statement: SYM ':' expr  {$$ = colonExpr($1, $3); }
	| expr  {$$ = $1; } ;

expr: SYM  {$$ = symExpr($1); }
	| NOT expr  {$$ = notExpr($2); }
	| expr AND expr  {$$ = opExpr(AND, $1, $3); }
	| expr OR expr  {$$ = opExpr(OR, $1, $3); }
	| expr XOR expr  {$$ = opExpr(XOR, $1, $3); }
	| expr THEN expr  {$$ = opExpr(THEN, $1, $3); }
	| expr EQ expr  {$$ = opExpr(EQ, $1, $3); }
	| '(' expr ')'  {$$ = parenExpr($2); }
	;
%%
void yyerror(char* s) {fprintf(stderr, "verity: %s\n", s); }

int main(int argc, char** argv) {
 int opt;
 while ((opt = getopt(argc, argv, "o:plte:sVh")) != -1) {
  switch (opt) {
   case 'o':
    if (freopen(optarg, "w", stdout) == NULL) {
     fprintf(stderr, "verity: %s: ", optarg); perror(NULL); return 2;
    }
    break;
   case 'p': flags.tblType = txtTbl; break;
   case 'l': flags.tblType = latexTbl; break;
   case 't': flags.tblType = texTbl; break;
   case 'e':
    if (!flags.eval) {
     flags.eval = 1; /*fclose(yyin);*/ yyin = tmpfile();
     /* Some operating systems object to trying to close stdin */
     if (!yyin) {
      fprintf(stderr, "verity: error creating temp file: "); perror(NULL);
      return 4;
     }
    }
    fputs(optarg, yyin); fputc('\n', yyin); break;
   case 's': flags.standalone = 1; break;
   case 'V':
    printf("Verity, a truth table generator, v.1.2.1\n"
     "Written by John T. Wodder II (minimiscience@users.sourceforge.net)\n"
     "Compiled %s, %s\n"
     "Verity is distributed under the GNU General Public License v.3\n"
     "Type `man verity' or `verity -h' for help\n",
     __DATE__, __TIME__);
    return 0;
   case 'h':
    printf("Usage: verity [-p | -t | -l] [-s] [-o outfile] [-e statements |"
     " infile]\n       verity [-h | -V]\n\n"
     "Options:\n"
     "\t-e  Treat statements as input\n"
     "\t-h  Print this summary of command-line options and exit\n"
     "\t-l  Output a LaTeX tabular\n"
     "\t-o  Write output to `outfile'\n"
     "\t-p  Output plain text\n"
     "\t-s  Output a complete Tex/LaTeX document\n"
     "\t-t  Output a TeX table\n"
     "\t-V  Print version information and exit\n");
    return 0;
   default:
    fprintf(stderr, "Usage: verity [-p | -t | -l] [-s] [-o outfile] [-e"
     " statements | infile]\n       verity [-h | -V]\n");
    return 2;
  }
 }
 if (flags.eval) rewind(yyin);
 else if (optind < argc) {
  yyin = fopen(argv[optind], "r");
  if (!yyin) {
   fprintf(stderr, "verity: %s: ", argv[optind]); perror(NULL); return 1;
  }
 }
 initLists();
 if (flags.standalone) printDocTop();
 return yyparse();
}