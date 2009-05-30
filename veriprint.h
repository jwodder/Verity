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

/* $Commit: $Format:%H$ $
 * $Date: $Format:%ai$ $
 * $Author: $Format:%an$ $
 */

#ifndef VERIPRINT_H
#define VERIPRINT_H
#include <stdio.h>
#include "veritypes.h"

void printDocTop(void);
void printDocEnd(void);
void printTbl(void);

void printLaTeXTbl(symbol** vars, int varno);
void printTeXTbl(symbol** vars, int varno);
void printTeXExp(expr* ex);

void printTxtTbl(symbol** vars, int varno);
int printTxtExp(expr* ex);

void printUTFTbl(symbol** vars, int varno);
int printUTFExp(expr* ex);

void printPSTbl(symbol** vars, int varno);
void printPSExp(expr* ex);
#endif
