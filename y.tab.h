
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
     SEMICOLON = 259,
     COMMA = 260,
     LPAREN = 261,
     RPAREN = 262,
     LCURL = 263,
     RCURL = 264,
     LTHIRD = 265,
     RTHIRD = 266,
     INT = 267,
     FLOAT = 268,
     VOID = 269,
     CONST_INT = 270,
     CONST_FLOAT = 271,
     FOR = 272,
     IF = 273,
     ELSE = 274,
     WHILE = 275,
     PRINTLN = 276,
     RETURN = 277,
     ASSIGNOP = 278,
     LOGICOP = 279,
     INCOP = 280,
     DECOP = 281,
     RELOP = 282,
     ADDOP = 283,
     MULOP = 284,
     NOT = 285,
     LOWER_THAN_ELSE = 286
   };
#endif
/* Tokens.  */
#define ID 258
#define SEMICOLON 259
#define COMMA 260
#define LPAREN 261
#define RPAREN 262
#define LCURL 263
#define RCURL 264
#define LTHIRD 265
#define RTHIRD 266
#define INT 267
#define FLOAT 268
#define VOID 269
#define CONST_INT 270
#define CONST_FLOAT 271
#define FOR 272
#define IF 273
#define ELSE 274
#define WHILE 275
#define PRINTLN 276
#define RETURN 277
#define ASSIGNOP 278
#define LOGICOP 279
#define INCOP 280
#define DECOP 281
#define RELOP 282
#define ADDOP 283
#define MULOP 284
#define NOT 285
#define LOWER_THAN_ELSE 286




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


