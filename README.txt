=== Verity - v.1.3 - README.txt ===

0. CONTENTS

0. Contents
1. About
2. File List
3. Version History


1. ABOUT

Verity is a small utility for generating truth tables of logical statements of
the sort that you would be assigned in a logic course.  Currently, it can
output tables as plain ASCII text, TeX or LaTeX markup, and PostScript.  See
the manpage for more information.

Verity was written by John T. Wodder II <jwodder@sdf.lonestar.org> and is
licensed under the GNU GPL, v.3 or later.  There's no official project site,
but, if you have a Gopher client, you should be able to find the latest version
at <gopher://sdf.lonestar.org/9/users/jwodder/src/verity-src.tgz>.


2. FILE LIST

COPYING - the GNU General Public License v.3, under which Verity is licensed
Makefile - the Makefile
README.txt - this file
TODO.txt - a list of some things that need to be done or could be done
verilex.c - the lexer
veriprint.c - contains the code for printing out tables
veriprint.h - the header file for veriprint.h
verity.1 - the manpage for Verity
verity.pod - the POD source for the manpage
verity.y - the Bison source for the parser
veritypes.c - contains code for storing expressions
veritypes.h - the header file for veritypes.c


3. VERSION HISTORY

22 Nov 2008 - v.1.3:
 - Version history started
 - Variables named 'x', 'X', or 'v' may now be used by preceding them with
   backslashes
 - Empty files & files containing only whitespace are no longer valid input
 - Code significantly improved
 - '|' now accepted as an alternative for 'v' and "||"
 - Location tracking implemented for error messages; Bison (not Yacc) now
   required for compiling
 - Syntax error messages made more descriptive; Bison now required even more
 - Added PostScript output
 - '=' and "==" now accepted as alternatives for "<->"
??? - v.1.4:
 - '+' now accepted as an alternative to 'x' and 'X'
 - The symbol to use for XOR in output can now be set by #defining or not
   #defining OLD_XOR_SYM in veriprint.c.  If it is #defined, the symbol will be
   'x' in plain text output and 'v' with a dot over it in (La)TeX and
   PostScript; if not #defined, it will be '+' in plain text and a circled '+'
   in (La)TeX and PostScript.
 - Implemented error recovery for invalid statements up to the next newline or
   semicolon
 - Added the -u and -U switches for producing Unicode output


$Id$
