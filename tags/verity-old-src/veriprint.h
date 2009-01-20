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

#ifndef VERIPRINT_H
#define VERIPRINT_H
#include <stdio.h>
#include "veritypes.h"
void printTbl(FILE* out);
void printLaTeXTbl(FILE* tex);
void printLaTeXExp(expr* ex, FILE* tex);
int exprLength(expr* ex);
void printTxtTbl(FILE* txt);
void printTxtExp(expr* ex, FILE* txt);
#endif
