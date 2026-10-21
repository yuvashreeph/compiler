/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     NUMBER = 259,
     WHILE = 260,
     FOR = 261,
     IF = 262,
     ELSE = 263,
     INT = 264,
     FLOAT = 265,
     DOUBLE = 266,
     CHAR = 267,
     VOID = 268,
     RETURN = 269,
     INC = 270,
     DEC = 271,
     LE = 272,
     GE = 273,
     EQ = 274,
     NE = 275,
     AND = 276,
     OR = 277,
     INVALID = 278,
     UMINUS = 279,
     LOWER_THAN_ELSE = 280
   };
#endif
/* Tokens.  */
#define ID 258
#define NUMBER 259
#define WHILE 260
#define FOR 261
#define IF 262
#define ELSE 263
#define INT 264
#define FLOAT 265
#define DOUBLE 266
#define CHAR 267
#define VOID 268
#define RETURN 269
#define INC 270
#define DEC 271
#define LE 272
#define GE 273
#define EQ 274
#define NE 275
#define AND 276
#define OR 277
#define INVALID 278
#define UMINUS 279
#define LOWER_THAN_ELSE 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 48 "parser.y"
{
    char *str;
}
/* Line 1529 of yacc.c.  */
#line 103 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

