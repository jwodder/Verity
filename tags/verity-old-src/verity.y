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
enum {txtTbl, latexTbl} tblType = txtTbl;
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
file: exprSet {printTbl(outfile); } | blocks | ;

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
 while ((opt = getopt(argc, argv, "o:tl")) != -1) {
  switch (opt) {
   case 'o':
    outfile = fopen(optarg, "w");
    if (!outfile) {
     fprintf(stderr, "Error: couldn't write to file `%s'\n", optarg); return 1;
    }
    break;
   case 't': tblType = txtTbl; break;
   case 'l': tblType = latexTbl; break;
  /* Add -e switch */
   default: return 0;  /* Do something else */
  }
 }
 if (optind < argc) {
  yyin = fopen(argv[optind], "r");
  if (!yyin) {
   fprintf(stderr, "Error: couldn't read file `%s'\n", argv[optind]); return 1;
  }
 }
 initLists();
 return yyparse();
}
