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
#include "veritypes.h"
#include "verity.tab.h"
void yyerror(char*);
%}
%%
\^|&&?  return AND;
v|\|\|  return OR;
[xX]  return XOR;
[-!~]  return NOT;
->  return THEN;
\<->  return EQ;
[:(){}]  return *yytext;
[[:alpha:]]  {yylval.sym = getSym(*yytext); return SYM; }
[;\n]  return EOL;
\\$  ;
[ \t\v\r\0]  ;
#.*$  ;
.  yyerror("unrecognized symbol");
%%
int yywrap(void) {return 1; }
