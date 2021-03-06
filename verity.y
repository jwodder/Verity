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

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>
#include <unistd.h>
#include "veritypes.h"
#include "veriprint.h"
void yyerror(char*, ...);
int yylex(void);
extern FILE* yyin;
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

%locations
%error-verbose
%destructor {freeExpr($$); } expr statement
%destructor {$$->refQty--; } SYM

%%

file : exprSet gap  {printTbl(); if (flags.standalone) printDocEnd(); }
     | blocks  gap  {if (flags.standalone) printDocEnd(); }
     ;

blocks : blocks gap '{' exprSet gap '}'  {printTbl(); clearLists(); }
       |        gap '{' exprSet gap '}'  {printTbl(); clearLists(); }
       ;

exprSet : exprSet gap EOL statement  {addStmnt($4); }
	| gap statement              {addStmnt($2); }
	;

gap: gap EOL | ;

statement : SYM ':' expr   {$$ = colonExpr($1, $3); }
	  | expr           {$$ = $1; }
	  | blight         {yyerrok; $$ = NULL; }
	  ;

blight : blight error
       | blight expr  {freeExpr($2); }
       | expr error   {freeExpr($1); }
       | error
       ;

expr : SYM             {$$ = symExpr($1); }
     | NOT expr        {$$ = notExpr($2); }
     | expr AND  expr  {$$ = opExpr(AND, $1, $3); }
     | expr OR   expr  {$$ = opExpr(OR, $1, $3); }
     | expr XOR  expr  {$$ = opExpr(XOR, $1, $3); }
     | expr THEN expr  {$$ = opExpr(THEN, $1, $3); }
     | expr EQ   expr  {$$ = opExpr(EQ, $1, $3); }
     | '(' expr ')'    {$$ = parenExpr($2); }
     ;

%%

void yyerror(char* s, ...) {
 fprintf(stderr, "verity: line %d, col. %d: ", yylloc.first_line,
  yylloc.first_column);
 va_list arg;
 va_start(arg, s);
 vfprintf(stderr, s, arg);
 va_end(arg);
 fputc('\n', stderr);
}

const char usage[] =
 "Usage: %s [-PptluU] [-s] [-o outfile] [-e statements | infile]\n"
 "       %s [-h | -V]\n";

int main(int argc, char** argv) {
 if (setlocale(LC_ALL, "") == NULL) perror("verity: error setting locale");
 int opt;
 while ((opt = getopt(argc, argv, "o:plte:sVhPuU")) != -1) {
  switch (opt) {
   case 'o':
    if (freopen(optarg, "w", stdout) == NULL) {
     fprintf(stderr, "verity: %s: ", optarg);
     perror(NULL);
     return 5;
    }
    break;
   case 'p': flags.tblType = txtTbl;   break;
   case 'u': flags.tblType = wideTbl;  break;
   case 'U': flags.tblType = utfTbl;   break;
   case 'l': flags.tblType = latexTbl; break;
   case 't': flags.tblType = texTbl;   break;
   case 'P': flags.tblType = psTbl;    break;
   case 'e':
    if (!flags.eval) {
     flags.eval = 1;
     yyin = tmpfile();
     if (!yyin) {perror("verity: error creating temp file"); return 4; }
    }
    fputs(optarg, yyin);
    fputc('\n', yyin);
    break;
   case 's': flags.standalone = 1; break;
   case 'V':
    printf("Verity, a truth table generator, v.1.4\n"
     "Written by John T. Wodder II (jwodder@sdf.lonestar.org)\n"
     "Compiled %s, %s\n"
     "Verity is distributed under the GNU General Public License v.3\n"
     "Type `man verity' or `verity -h' for help\n",
     __DATE__, __TIME__);
    return 0;
   case 'h':
    printf(usage, argv[0], argv[0]);
    puts("\nOptions:\n"
	   "  -e statements - Treat `statements' as input\n"
	   "  -h - Print this summary of command-line options and exit\n"
	   "  -l - Output a LaTeX tabular\n"
	   "  -o outfile - Write output to `outfile'\n"
	   "  -P - Output a PostScript document\n"
	   "  -p - Output plain text\n"
	   "  -s - Output a complete Tex/LaTeX document\n"
	   "  -t - Output a TeX table\n"
	   "  -u - Output plain text with Unicode operators\n"
	   "  -U - Output a Unicode table\n"
	   "  -V - Print version information and exit");
    return 0;
   default: fprintf(stderr, usage, argv[0], argv[0]); return 2;
  }
 }
 if (flags.eval) rewind(yyin);
 else if (optind < argc) {
  yyin = fopen(argv[optind], "r");
  if (!yyin) {
   fprintf(stderr, "verity: %s: ", argv[optind]);
   perror(NULL);
   return 5;
  }
 }
 if (flags.tblType == psTbl) flags.standalone = 1;
 if (flags.standalone) printDocTop();
 return yyparse();
}
