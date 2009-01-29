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
#include <ctype.h>
#include <errno.h>
#include "veritypes.h"
#include "verity.tab.h"
extern void yyerror(char*, ...);

FILE* yyin;

int yylex(void) {
 if (yyin == NULL) yyin = stdin;
 /* yyin is initialized here because I can't set it to a non-const value above.
  */
 if (feof(yyin)) return 0;
 else if (ferror(yyin)) {
  yyerror("error reading input: %s", strerror(errno));
  exit(3);
 }
 for (;;) {
  int ch = fgetc(yyin);
  yylloc.first_column++;
  switch (ch) {
   case '^': return AND;
   case '&':
    ch = fgetc(yyin);
    if (ch != '&') ungetc(ch, yyin);
    else yylloc.first_column++;
    return AND;
   case 'v': return OR;
   case '|':
    ch = fgetc(yyin);
    if (ch != '|') ungetc(ch, yyin);
    else yylloc.first_column++;
    return OR;
   case 'x': case 'X': case '+': return XOR;
   case '-': 
    ch = fgetc(yyin);
    if (ch == '>') {yylloc.first_column++; return THEN; }
    else ungetc(ch, yyin);  /* ... and then fall through */
   case '!': case '~': return NOT;
   case '<':
    ch = fgetc(yyin);
    yylloc.first_column++;
    if (ch == '-') {
     ch = fgetc(yyin);
     yylloc.first_column++;
     if (ch == '>') return EQ;
     else if (ch == EOF) {
      yyerror("unexpected end of file while reading token");
      exit(3);
     } else {yyerror("malformed symbol \"<-%c\"", ch); exit(3); }
    } else if (ch == EOF) {
     yyerror("unexpected end of file while reading token");
     exit(3);
    } else {yyerror("malformed symbol \"<%c\"", ch); exit(3); }
   case '=':
    ch = fgetc(yyin);
    if (ch != '=') ungetc(ch, yyin);
    else yylloc.first_column++;
    return EQ;
   case ':': case '(': case ')': case '{': case '}': return ch;
   case '\n':
    yylloc.first_line++;
    yylloc.first_column = 0;
    /* and fall through */
   case ';':
    return EOL;
   case '#':
    while (ch != '\n' && ch != EOF) {ch = fgetc(yyin); yylloc.first_column++; }
    if (ferror(yyin)) {
     yyerror("error reading input: %s", strerror(errno));
     exit(3);
    }
    yylloc.first_line++;
    yylloc.first_column = 0;
    break;
   case '\\':
    ch = fgetc(yyin);
    yylloc.first_column++;
    if (ch == '\n') {
     yylloc.first_line++;
     yylloc.first_column = 0;
     break;
    } else if (ch == EOF) {
     if (ferror(yyin)) {
      yyerror("error reading input: %s", strerror(errno));
      exit(3);
     }
     fprintf(stderr, "verity: warning: invalid backslash before end-of-file\n");
     return 0;
    } else if (isalpha(ch)) {
     yylval.sym = getSym(ch);
     return SYM;
    } else {yyerror("invalid escape sequence \"\\%c\"", ch); exit(3); }
   case EOF:
    if (feof(yyin)) return 0;
    else {yyerror("error reading input: %s", strerror(errno)); exit(3); }
   default:
    if (isspace(ch)) break;
    else if (isalpha(ch)) {yylval.sym = getSym(ch); return SYM; }
    else {yyerror("unrecognized symbol `%c'", ch); exit(3); }
  }
 }
}
