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
FILE* outfile;

struct {
 enum {txtTbl=0, latexTbl, texTbl} tblType;
 _Bool eval, standalone;
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
file: exprSet {printTbl(outfile); if (flags.standalone) printDocEnd(outfile); }
	| blocks {if (flags.standalone) printDocEnd(outfile); } | ;

blocks: blocks '{' exprSet '}'  {printTbl(outfile); clearLists(); }
	| '{' exprSet '}'  {printTbl(outfile); clearLists(); }
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
void yyerror(char* s) {fprintf(stderr, "Error: %s\n", s); }

int main(int argc, char** argv) {
 outfile = stdout;
 int opt;
 while ((opt = getopt(argc, argv, "o:plte:sVh")) != -1) {
  switch (opt) {
   case 'o':
    if (!flags.eval) {
     outfile = fopen(optarg, "w");
     if (!outfile) {
      fprintf(stderr, "Error: couldn't write to file `%s'\n", optarg); return 1;
     }
    }
    break;
   case 'p': flags.tblType = txtTbl; break;
   case 'l': flags.tblType = latexTbl; break;
   case 't': flags.tblType = texTbl; break;
   case 'e':
    if (!flags.eval) {
     flags.eval = 1; fclose(yyin); yyin = tmpfile();
     if (!yyin) {
      fprintf(stderr, "Error: couldn't create temp file for `-e' arguments\n");
      return 1;
     }
    }
    fputs(optarg, yyin); fputc('\n', yyin); break;
   case 's': flags.standalone = 1; break;
   case 'V':
    printf("Verity, version 1.2, by John T. Wodder II\n"); /* Add more? */
    printf("Compiled %s, %s\n", __DATE__, __TIME__);
    printf("See `man verity' or `verity -h' for help'\n");
    return 0;
   case 'h':
    printf("Verity, a truth table generator\n\n");
    printf("Usage: verity [-p | -t | -l] [-s] [-o outfile] [-e statements |"
     " infile]\n");
    printf("Options:\n -p  Output plain text\n -t  Output a TeX table\n"
     " -l  Output a LaTeX tabular\n -s  Output a complete Tex/LaTeX document\n"
     " -o  Write output to outfile\n -e  Make a table of the statements\n\n");
    printf("See `man verity' for information on the syntax of statements.\n");
    return 0;
   default:
    fprintf(stderr, "Verity: unknown option `-%c'\n"
     "See `man verity' or `verity -h' for help\n", optopt);
    return 1;
  }
 }
 if (flags.eval) rewind(yyin);
 else if (optind < argc) {
  yyin = fopen(argv[optind], "r");
  if (!yyin) {
   fprintf(stderr, "Error: couldn't read file `%s'\n", argv[optind]); return 1;
  }
 }
 initLists();
 if (flags.standalone) printDocTop(outfile);
 return yyparse();
}
