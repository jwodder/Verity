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
/* yyin is initialized at the beginning of main() because I can't set it to a
 * non-const value here. */

int yylex(void) {
 if (feof(yyin)) return 0;
 else if (ferror(yyin)) {
  yyerror("error reading input: %s", strerror(errno));
  exit(3);
 }
 int ch;
 for (;;) {
  switch (ch = fgetc(yyin)) {
   case '^': return AND;
   case '&':
    ch = fgetc(yyin);
    if (ch != '&') ungetc(ch, yyin);
    return AND;
   case 'v': return OR;
   case '|':
    ch = fgetc(yyin);
    if (ch != '|') ungetc(ch, yyin);
    return OR;
   case 'x': case 'X': return XOR;
   case '-': 
    ch = fgetc(yyin);
    if (ch == '>') return THEN;
    else ungetc(ch, yyin);  /* ... and then fall through */
   case '!': case '~': return NOT;
   case '<':
    ch = fgetc(yyin);
    if (ch == '-') {
     ch = fgetc(yyin);
     if (ch == '>') return EQ;
     else if (ch == EOF) {
      yyerror("unexpected end of file while reading token");
      exit(3);
     } else {yyerror("malformed symbol \"<-%c\"", ch); exit(3); }
    } else if (ch == EOF) {
     yyerror("unexpected end of file while reading token");
     exit(3);
    } else {yyerror("malformed symbol \"<%c\"", ch); exit(3); }
   case ':': case '(': case ')': case '{': case '}': return ch;
   case ';': case '\n': return EOL;
   case '#':
    while (ch != '\n' && ch != EOF) ch = fgetc(yyin);
    if (ferror(yyin)) {
     yyerror("error reading input: %s", strerror(errno));
     exit(3);
    }
    break;
   case '\\':
    ch = fgetc(yyin);
    if (ch == '\n') break;
    else if (ch == EOF) {
     fprintf(stderr, "verity: warning: invalid backslash before end-of-file\n");
     return 0;
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
