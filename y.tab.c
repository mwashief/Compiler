
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "1605058.y"


#include<iostream>
#include<cstdlib>
#include<vector>
#include<cstring>
#include<cmath>
#include<string>
#include<fstream>
#include<sstream>
#include "symboltable.cpp"
#define YYSTYPE SymbolInfo*

using namespace std;

int yyparse(void);
int yylex(void);
FILE *codeout;
FILE *logout, *fp;
extern FILE *yyin;
extern int line_count;
extern int error;
extern char* yytext;


SymbolTable *table = new SymbolTable(50);
SymbolInfo* params = 0;
vector<SymbolInfo*> functions;
vector<SymbolInfo*> variables;

string returnType = "";
string currentFunc = "";
int labelCount=1;
int tempCount=1;



string backup(SymbolInfo* s)
{
	string str = "";
	if(s == 0) return str;
	if(s->alist.size() != 0)str = ";backup " + s->name + " function temporaries\n";
	for(int i=0; i<s->alist.size(); i++)
	{
		str = str + "push " + s->alist[i]->tag + "\n";
	}
	return str;
}

string restore(SymbolInfo* s)
{
	string str = "";
	if(s == 0) return str;
	if(s->alist.size() != 0)str = ";restore " + s->name + " function temporaries\n";
	for(int i=s->alist.size()-1 ; i>=0; i--)
	{
		str = str + "pop " + s->alist[i]->tag + "\n";
	}
	return str;
}

string mov(string reg, SymbolInfo* s)
{
	return "mov " + reg + ", " + s->tag + "\n";
}

string mov(string reg, SymbolInfo* s, string offset)
{
	return "mov " + reg + ", " + s->tag + "[" + offset + "]\n";
}

string mov(SymbolInfo* s, string reg)
{
	return "mov " + s->tag  + ", " + reg + "\n";
}

string newLabel()
{
	char *lb= new char[4];
	strcpy(lb,"L");
	char b[3];
	sprintf(b,"%d", labelCount);
	labelCount++;
	strcat(lb,b);
	return string(lb);
}

string newTemp()
{
	char *t= new char[4];
	strcpy(t,"t");
	char b[3];
	sprintf(b,"%d", tempCount);
	tempCount++;
	strcat(t,b);
	return string(t);
}

string newTempInsert()
{
	string s = newTemp();
	SymbolInfo* sip = new SymbolInfo("","");
	sip->tag = s;
	variables.push_back(sip);
	return s;
}

void yyerror(char *s)
{
	error++;
	fprintf(logout, "Error at Line %d: Syntax error\n", line_count);
}

vector<string> split(string s)
{
    vector<string> result;
    istringstream iss(s);
    for(string t; iss >> t; )result.push_back(t);
    return result;
}

void peephole()
{
    ifstream ifs("code.asm");
    vector<string> preLines;
    string curLine = "";
    //int i = 0;
    while(getline(ifs, curLine))
    {
        //i++;
        if(curLine.size() <= 1) continue;
        if(curLine[0] == ';') continue;
        if(preLines.size()<1 )
        {
            preLines.push_back(curLine);
            continue;
        }
        vector<string> preTokens = split(preLines[preLines.size()-1]);
        vector<string> curTokens = split(curLine);
        if((preTokens[0] == "push" && curTokens[0] == "pop") || (preTokens[0] == "pop" && curTokens[0] == "push"))
        {
            if(preTokens[1] == curTokens[1])
            {
                //cout << "Stack: " << i << endl;
                preLines.pop_back();
                continue;
            }
        }
        else if(preTokens[0] == "mov" && curTokens[0] == "mov")
        {
            //cout << "found mov pair" << i << endl;
            curTokens[1].erase(curTokens[1].end()-1, curTokens[1].end());
            preTokens[1].erase(preTokens[1].end()-1, preTokens[1].end());
            //cout << ": " << preTokens[1] << " " << preTokens[2] << " | " << curTokens[1] << " " << curTokens[2] <<endl;
            if((curTokens[1] == preTokens[2]) && (curTokens[2] == preTokens[1]))
            {
                //cout << "MOV: " << i << endl;
                continue;
            }
        }
        preLines.push_back(curLine);
    }
    ifs.close();
    ofstream ofs("code.asm");
    for(int i=0; i<preLines.size(); i++)
    {
        //cout << preLines[i] << endl;
        ofs << preLines[i] << endl;
    }
}



/* Line 189 of yacc.c  */
#line 247 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 351 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   145

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNRULES -- Number of states.  */
#define YYNSTATES  122

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   286

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    16,    23,
      29,    35,    40,    43,    46,    51,    55,    58,    60,    64,
      67,    69,    71,    75,    77,    79,    81,    85,    92,    94,
      99,   101,   104,   106,   108,   110,   118,   124,   132,   138,
     144,   148,   150,   153,   155,   160,   162,   166,   168,   172,
     174,   178,   180,   184,   186,   190,   193,   196,   198,   200,
     205,   209,   211,   213,   216,   219,   221,   222,   226
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      33,     0,    -1,    34,    -1,    34,    35,    -1,    35,    -1,
      44,    -1,    36,    -1,    39,    -1,    45,     3,     6,    40,
       7,     4,    -1,    45,     3,     6,     7,     4,    -1,    45,
       3,     6,    40,     7,    -1,    45,     3,     6,     7,    -1,
      37,    41,    -1,    38,    41,    -1,    40,     5,    45,     3,
      -1,    40,     5,    45,    -1,    45,     3,    -1,    45,    -1,
      42,    47,    43,    -1,    42,    43,    -1,     8,    -1,     9,
      -1,    45,    46,     4,    -1,    12,    -1,    13,    -1,    14,
      -1,    46,     5,     3,    -1,    46,     5,     3,    10,    15,
      11,    -1,     3,    -1,     3,    10,    15,    11,    -1,    48,
      -1,    47,    48,    -1,    44,    -1,    49,    -1,    41,    -1,
      17,     6,    49,    49,    51,     7,    48,    -1,    18,     6,
      51,     7,    48,    -1,    18,     6,    51,     7,    48,    19,
      48,    -1,    20,     6,    51,     7,    48,    -1,    21,     6,
       3,     7,     4,    -1,    22,    51,     4,    -1,     4,    -1,
      51,     4,    -1,     3,    -1,     3,    10,    51,    11,    -1,
      52,    -1,    50,    23,    52,    -1,    53,    -1,    53,    24,
      53,    -1,    54,    -1,    54,    27,    54,    -1,    55,    -1,
      54,    28,    55,    -1,    56,    -1,    55,    29,    56,    -1,
      28,    56,    -1,    30,    56,    -1,    57,    -1,    50,    -1,
       3,     6,    58,     7,    -1,     6,    51,     7,    -1,    15,
      -1,    16,    -1,    50,    25,    -1,    50,    26,    -1,    59,
      -1,    -1,    59,     5,    52,    -1,    52,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   187,   187,   226,   232,   238,   242,   248,   254,   272,
     291,   359,   420,   440,   461,   469,   476,   485,   496,   500,
     507,   529,   534,   572,   576,   580,   586,   591,   597,   603,
     612,   618,   626,   630,   634,   638,   652,   661,   674,   687,
     694,   713,   718,   724,   740,   764,   775,   809,   813,   856,
     860,   936,   940,   963,   967,  1004,  1008,  1013,  1034,  1038,
    1086,  1090,  1096,  1102,  1114,  1128,  1133,  1139,  1148
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "SEMICOLON", "COMMA", "LPAREN",
  "RPAREN", "LCURL", "RCURL", "LTHIRD", "RTHIRD", "INT", "FLOAT", "VOID",
  "CONST_INT", "CONST_FLOAT", "FOR", "IF", "ELSE", "WHILE", "PRINTLN",
  "RETURN", "ASSIGNOP", "LOGICOP", "INCOP", "DECOP", "RELOP", "ADDOP",
  "MULOP", "NOT", "LOWER_THAN_ELSE", "$accept", "start", "program", "unit",
  "func_declaration", "func_definition_head_one",
  "func_definition_head_two", "func_definition", "parameter_list",
  "compound_statement", "lcurl", "rcurl", "var_declaration",
  "type_specifier", "declaration_list", "statements", "statement",
  "expression_statement", "variable", "expression", "logic_expression",
  "rel_expression", "simple_expression", "term", "unary_expression",
  "factor", "argument_list", "arguments", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    35,    36,    36,
      37,    38,    39,    39,    40,    40,    40,    40,    41,    41,
      42,    43,    44,    45,    45,    45,    46,    46,    46,    46,
      47,    47,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    49,    49,    50,    50,    51,    51,    52,    52,    53,
      53,    54,    54,    55,    55,    56,    56,    56,    57,    57,
      57,    57,    57,    57,    57,    58,    58,    59,    59
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     6,     5,
       5,     4,     2,     2,     4,     3,     2,     1,     3,     2,
       1,     1,     3,     1,     1,     1,     3,     6,     1,     4,
       1,     2,     1,     1,     1,     7,     5,     7,     5,     5,
       3,     1,     2,     1,     4,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     2,     2,     1,     1,     4,
       3,     1,     1,     2,     2,     1,     0,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    23,    24,    25,     0,     2,     4,     6,     0,     0,
       7,     5,     0,     1,     3,    20,    12,     0,    13,    28,
       0,    43,    41,     0,    21,    61,    62,     0,     0,     0,
       0,     0,     0,     0,    34,    19,    32,     0,     0,    30,
      33,    58,     0,    45,    47,    49,    51,    53,    57,     0,
       0,    22,     0,    66,     0,     0,     0,     0,     0,     0,
       0,    58,    55,    56,    28,    18,    31,     0,    63,    64,
      42,     0,     0,     0,     0,    11,     0,    17,     0,    26,
      68,     0,    65,     0,    60,     0,     0,     0,     0,    40,
      46,    48,    50,    52,    54,     9,     0,    10,    16,    29,
       0,    59,     0,    44,     0,     0,     0,     0,    15,     8,
       0,    67,     0,    36,    38,    39,    14,    27,     0,     0,
      35,    37
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,     8,     9,    10,    76,    34,
      17,    35,    36,    37,    20,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    81,    82
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -53
static const yytype_int16 yypact[] =
{
      46,   -53,   -53,   -53,    10,    46,   -53,   -53,    13,    13,
     -53,   -53,    20,   -53,   -53,   -53,   -53,    85,   -53,    16,
      60,    33,   -53,    22,   -53,   -53,   -53,    25,    28,    34,
      49,    22,    22,    22,   -53,   -53,   -53,    59,    85,   -53,
     -53,    31,    70,   -53,    51,    39,    48,   -53,   -53,     1,
      63,   -53,    76,    22,    22,    73,    14,    22,    22,    78,
      81,    43,   -53,   -53,    77,   -53,   -53,    22,   -53,   -53,
     -53,    22,    22,    22,    22,    82,    56,    87,    84,    94,
     -53,   101,   111,   100,   -53,    14,   110,   118,   122,   -53,
     -53,   -53,   102,    48,   -53,   -53,    46,    88,   -53,   -53,
     116,   -53,    22,   -53,    22,   106,   106,   128,   130,   -53,
     124,   -53,   131,   120,   -53,   -53,   -53,   -53,   106,   106,
     -53,   -53
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -53,   -53,   -53,   132,   -53,   -53,   -53,   -53,   -53,    64,
     -53,   103,    19,     0,   -53,   -53,   -35,   -52,   -26,   -22,
     -51,    69,    71,    72,   -21,   -53,   -53,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      12,    55,    80,    66,    85,    12,    61,    61,    75,    60,
      13,    62,    63,     1,     2,     3,    90,    21,    22,    11,
      23,    15,    49,    19,    11,    21,    50,    61,    23,    25,
      26,    56,    83,   104,    57,    86,    87,    25,    26,    53,
      58,    61,    32,    54,    33,    61,    61,    61,    61,    77,
      32,   111,    33,    94,    67,    59,    68,    69,     1,     2,
       3,    96,    64,    97,    51,    52,    72,    73,    68,    69,
     113,   114,    16,    18,    70,    71,    61,    74,    78,    79,
      84,    88,   112,   120,   121,    89,    95,    50,    21,    22,
      98,    23,   109,    15,    24,    99,   108,     1,     2,     3,
      25,    26,    27,    28,   100,    29,    30,    31,   101,    21,
      22,   103,    23,    32,    15,    33,   102,   105,     1,     2,
       3,    25,    26,    27,    28,   106,    29,    30,    31,   107,
      73,   110,   115,   116,    32,   117,    33,    14,   118,   119,
      91,    65,     0,    92,     0,    93
};

static const yytype_int8 yycheck[] =
{
       0,    23,    53,    38,    56,     5,    32,    33,     7,    31,
       0,    32,    33,    12,    13,    14,    67,     3,     4,     0,
       6,     8,     6,     3,     5,     3,    10,    53,     6,    15,
      16,     6,    54,    85,     6,    57,    58,    15,    16,     6,
       6,    67,    28,    10,    30,    71,    72,    73,    74,    49,
      28,   102,    30,    74,    23,     6,    25,    26,    12,    13,
      14,     5,     3,     7,     4,     5,    27,    28,    25,    26,
     105,   106,     8,     9,     4,    24,   102,    29,    15,     3,
       7,     3,   104,   118,   119,     4,     4,    10,     3,     4,
       3,     6,     4,     8,     9,    11,    96,    12,    13,    14,
      15,    16,    17,    18,    10,    20,    21,    22,     7,     3,
       4,    11,     6,    28,     8,    30,     5,     7,    12,    13,
      14,    15,    16,    17,    18,     7,    20,    21,    22,     7,
      28,    15,     4,     3,    28,    11,    30,     5,     7,    19,
      71,    38,    -1,    72,    -1,    73
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    12,    13,    14,    33,    34,    35,    36,    37,    38,
      39,    44,    45,     0,    35,     8,    41,    42,    41,     3,
      46,     3,     4,     6,     9,    15,    16,    17,    18,    20,
      21,    22,    28,    30,    41,    43,    44,    45,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     6,
      10,     4,     5,     6,    10,    51,     6,     6,     6,     6,
      51,    50,    56,    56,     3,    43,    48,    23,    25,    26,
       4,    24,    27,    28,    29,     7,    40,    45,    15,     3,
      52,    58,    59,    51,     7,    49,    51,    51,     3,     4,
      52,    53,    54,    55,    56,     4,     5,     7,     3,    11,
      10,     7,     5,    11,    49,     7,     7,     7,    45,     4,
      15,    52,    51,    48,    48,     4,     3,    11,     7,    19,
      48,    48
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 188 "1605058.y"
    {
     	(yyval) =(yyvsp[(1) - (1)]);
     	SymbolInfo *s = table->lookUp("main");
     	if(s == 0 || s->size != -2)
     	{
     		fprintf(logout, "Error at Line %d: main function is not declared\n", line_count);
     		error++;
     		yyerror("Error");
     	}
     	for(int i=0; i<table->current->buckets; i++)
        {
            SymbolInfo *temp;
            temp = table->current->scope[i];
            if(temp == 0) continue;
            while(temp!=0)
            {
                if(temp->size == -1)
                {
                    fprintf(logout, "Error at Line %d: %s function is not defined.\n", line_count, temp->name.c_str());
                    error++;
                }
                temp = temp->next;
            }
        }
        fprintf(codeout, ".model small\n.stack 100h\n.data\n");
        for(int i=0; i<variables.size(); i++)
        {
        	fprintf(codeout, "%s dw %d dup(?)\n", variables[i]->tag.c_str(), variables[i]->size>0?variables[i]->size:1);
        }
        fprintf(codeout, ".code\n");
        for(int i=0; i<functions.size(); i++)
        {
        	fprintf(codeout, "%s proc\n\n%s\n%s endp\n\n", functions[i]->name.c_str(), functions[i]->description.c_str(), functions[i]->name.c_str());
        }
        fprintf(codeout, "end main");
     }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 227 "1605058.y"
    {
     	(yyvsp[(1) - (2)])->description = (yyvsp[(1) - (2)])->description + "\n" + (yyvsp[(2) - (2)])->description + "\n";
     	(yyval) = (yyvsp[(1) - (2)]);
     	
     }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 233 "1605058.y"
    {
     	(yyval) = (yyvsp[(1) - (1)]);
     }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 239 "1605058.y"
    {
     	(yyval) = (yyvsp[(1) - (1)]);
     }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 243 "1605058.y"
    {
     	(yyval) = (yyvsp[(1) - (1)]);
     	params = 0;

     }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 249 "1605058.y"
    {
     	(yyval) = (yyvsp[(1) - (1)]);
     }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 255 "1605058.y"
    {
			SymbolInfo* s= new SymbolInfo((yyvsp[(2) - (6)])->name, (char *)"ID");
			s->type = (yyvsp[(1) - (6)])->type;
			s->alist = (yyvsp[(4) - (6)])->alist;
			s->description = (yyvsp[(1) - (6)])->description + " " + (yyvsp[(2) - (6)])->description + "(" + (yyvsp[(4) - (6)])->description + ");";
			(yyval) = s;
			if(table->lookUp((yyval)->name))
			{
				fprintf(logout, "Error at Line %d: Multiple Declaration of function %s\n", line_count, (yyval)->name.c_str());
				error++;
			}
			else
			{
				table->insert((yyval));
				(yyval)->size = -1;
			}
		}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 273 "1605058.y"
    {
			SymbolInfo* s= new SymbolInfo((yyvsp[(2) - (5)])->name, (char *)"ID");
			s->type = (yyvsp[(1) - (5)])->type;
			s->description = (yyvsp[(1) - (5)])->description + " " + (yyvsp[(2) - (5)])->description + "();";
			(yyval) = s;
			if(table->lookUp((yyval)->name))
			{
				fprintf(logout, "Error at Line %d: Multiple Declaration of function %s\n", line_count, (yyval)->name.c_str());
				error++;
			}
			else
			{
				table->insert((yyval));
				(yyval)->size = -1;
			}
		}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 292 "1605058.y"
    {
			SymbolInfo* s= new SymbolInfo((yyvsp[(2) - (5)])->name, (char *)"ID");
			s->type = (yyvsp[(1) - (5)])->type;
			for(int i=0; i<(yyvsp[(4) - (5)])->alist.size(); i++)
			{
				SymbolInfo* temp = new SymbolInfo((yyvsp[(4) - (5)])->alist[i]->name, (yyvsp[(4) - (5)])->alist[i]->type);
				temp->tag = (yyvsp[(4) - (5)])->alist[i]->tag;
				s->alist.push_back(temp);
			}
			(yyval) = s;
			

			SymbolInfo* temp = table->lookUp((yyval)->name);
			
			if(temp)
			{
				if(temp->size>=0)
				{
					fprintf(logout, "Error at Line %d: Already there is a variable with this name \"%s\"\n", line_count, (yyvsp[(2) - (5)])->name.c_str());
					error++;
				}
				else
				{
					
					if (temp->type != (yyval)->type)
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition return type mismatch\n", line_count, (yyvsp[(2) - (5)])->name.c_str());
						error++;
					}
					if(temp->size == -2)
					{
						fprintf(logout, "Error at Line %d: Multiple defination of function %s\n", line_count, (yyval)->name.c_str());
						error++;
					}
					if (temp->alist.size() != (yyval)->alist.size())
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters differ in number\n", line_count, (yyvsp[(2) - (5)])->name.c_str());
						error++;
					}
					int i;
					for(i=0; i<temp->alist.size(); i++)
					{
						if(temp->alist[i]->type != (yyval)->alist[i]->type) break;
					}
					if(i == temp->alist.size())
					{
						table->insert((yyval));
						(yyval)->size = -2;
					}
					else
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters mismatch\n", line_count, (yyvsp[(2) - (5)])->name.c_str());
						error++;
					}
				}
			}
			else
			{
				table->insert((yyval));
				(yyval)->size = -2;
			}

			returnType = (yyvsp[(1) - (5)])->type;
			currentFunc = (yyvsp[(2) - (5)])->name;
		}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 360 "1605058.y"
    {
			SymbolInfo* s= new SymbolInfo((yyvsp[(2) - (4)])->name, (char *)"ID");
			s->type = (yyvsp[(1) - (4)])->type;
			(yyval) = s;
			SymbolInfo* temp = table->lookUp((yyval)->name);

			if(temp)
			{
				if(temp->size>=0)
				{
					fprintf(logout, "Error at Line %d: Already there is a variable with this name \"%s\"\n", line_count, (yyvsp[(2) - (4)])->name.c_str());
					error++;
				}
				else
				{
					if (temp->type != (yyval)->type)
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition return type mismatch\n", line_count, (yyvsp[(2) - (4)])->name.c_str());
						error++;
					}
					if(temp->size == -2)
					{
						fprintf(logout, "Error at Line %d: Multiple defination of function %s\n", line_count, (yyval)->name.c_str());
						error++;
					}
					if (temp->alist.size() != (yyval)->alist.size())
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters differ in number\n", line_count, (yyvsp[(2) - (4)])->name.c_str());
						error++;
					}
					
					int i;
					for(i=0; i<temp->alist.size(); i++)
					{
						if(temp->alist[i]->type != (yyval)->alist[i]->type) break;
					}
					if(i == temp->alist.size())
					{
						table->insert((yyval));
						(yyval)->size = -2;
					}
					else
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters mismatch\n", line_count, (yyvsp[(2) - (4)])->name.c_str());
						error++;
					}
				}
			}
			else
			{
				table->insert((yyval));
				(yyval)->size = -2;
			}

			returnType = (yyvsp[(1) - (4)])->type;
			currentFunc = (yyvsp[(2) - (4)])->name;
		}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 421 "1605058.y"
    {
				(yyvsp[(1) - (2)])->description = (yyvsp[(2) - (2)])->description;
				(yyval) = (yyvsp[(1) - (2)]);
     			returnType = "";
     			SymbolInfo* temp = new SymbolInfo((yyval)->name, (yyval)->type);
     			temp->description = (yyval)->description;
     			if((yyval)->name == "main")
     			{
     				temp->description = "mov ax, @data\nmov ds, ax\n\n" + temp->description + "\nmov ah, 4ch\nint 21h\n";
     				functions[0] = temp;
     			}
     			else
     			{
     				functions.push_back(temp);
     			}
     			SymbolInfo* s = table->lookUp(currentFunc);
     			
			}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 441 "1605058.y"
    {
				(yyvsp[(1) - (2)])->description = (yyvsp[(2) - (2)])->description;
				(yyval) = (yyvsp[(1) - (2)]);
     			returnType = "";
     			SymbolInfo* temp = new SymbolInfo((yyval)->name, (yyval)->type);
     			temp->description = (yyval)->description;
     			if((yyval)->name == "main")
     			{
     				temp->description = "mov ax, @data\nmov ds, ax\n\n" + temp->description + "\nmov ah, 4ch\nint 21h\n";
     				functions[0] = temp;
     			}
     			else
     			{
     				functions.push_back(temp);
     			}
     			SymbolInfo* s = table->lookUp(currentFunc);
			}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 462 "1605058.y"
    {
			(yyvsp[(3) - (4)])->name = (yyvsp[(4) - (4)])->name;
			(yyvsp[(3) - (4)])->tag = newTempInsert();
			(yyvsp[(1) - (4)])->alist.push_back((yyvsp[(3) - (4)]));
			(yyval) = (yyvsp[(1) - (4)]);
			params = (yyval);
		}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 470 "1605058.y"
    {
			(yyvsp[(3) - (3)])->tag = newTempInsert();
			(yyvsp[(1) - (3)])->alist.push_back((yyvsp[(3) - (3)]));
			(yyval) = (yyvsp[(1) - (3)]);
			params = (yyval);
		}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 477 "1605058.y"
    {
 			(yyvsp[(1) - (2)])->name = (yyvsp[(2) - (2)])->name;
 			(yyvsp[(1) - (2)])->tag = newTempInsert();
 			SymbolInfo* s = new SymbolInfo();
 			s->alist.push_back((yyvsp[(1) - (2)]));
 			(yyval) = s;
 			params = (yyval);
 		}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 486 "1605058.y"
    {
			SymbolInfo* s = new SymbolInfo();
 			s->alist.push_back((yyvsp[(1) - (1)]));
 			(yyvsp[(1) - (1)])->tag = newTempInsert();
 			(yyval) = s;
 			params = (yyval);
		}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 497 "1605058.y"
    {
				(yyval) = (yyvsp[(2) - (3)]);
			}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 501 "1605058.y"
    {
 		    	(yyval) = new SymbolInfo();
 		    	(yyval)->description = "";
   		    }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 508 "1605058.y"
    {
				table->enterScope();
				if(params)
				{
					for(int i=0; i<params->alist.size(); i++)
					{
						if(table->current->lookUp(params->alist[i]->name))
						{
							fprintf(logout, "Error at Line %d: Multiple Declaration of %s\n", line_count, params->alist[i]->name.c_str());
							error++;
						}
						else if(params->alist[i]->name != "")
						{
							table->insert(params->alist[i]);
						}
					}
				}
				params = 0;
			}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 530 "1605058.y"
    {
				table->exitScope();
			}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 535 "1605058.y"
    {
				if((yyvsp[(1) - (3)])->type != "VOID")
				{
					(yyvsp[(2) - (3)])->type = (yyvsp[(1) - (3)])->type;
					(yyval) = (yyvsp[(2) - (3)]);
					for(int i=0; i<(yyval)->alist.size(); i++)
					{
						(yyval)->alist[i]->type = (yyvsp[(1) - (3)])->type;
						if((yyval)->alist[i]->name == "main")
						{
							fprintf(logout, "Error at Line %d: Variable name cannot be main\n", line_count);
							error++;
						}
						else if(table->current->lookUp((yyval)->alist[i]->name))
						{
							fprintf(logout, "Error at Line %d: Multiple Declaration of %s\n", line_count, (yyval)->alist[i]->name.c_str());
							error++;
						}
						else
						{
							(yyval)->alist[i]->tag = newTemp();
							table->insert((yyval)->alist[i]);
							SymbolInfo* temp = new SymbolInfo((yyval)->alist[i]->name, (yyval)->alist[i]->type);
							temp->tag = (yyval)->alist[i]->tag;
							temp->size = (yyval)->alist[i]->size;
							variables.push_back(temp);
						}
					}
				}
				else
				{
					fprintf(logout, "Error at Line %d: variable declared as void\n", line_count);
					error++;
				}
			}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 573 "1605058.y"
    { 
			 	SymbolInfo* s = new SymbolInfo((char *)"",(char *)"CONST_INT"); s->description = "int"; (yyval) = s;
			 }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 577 "1605058.y"
    { 
 				SymbolInfo* s = new SymbolInfo((char *)"",(char *)"CONST_FLOAT"); s->description = "float"; (yyval) = s;
 			}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 581 "1605058.y"
    { 
 				SymbolInfo* s = new SymbolInfo((char *)"",(char *)"VOID"); s->description = "void"; (yyval) = s;
 			}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 587 "1605058.y"
    {
				(yyvsp[(1) - (3)])->alist.push_back((yyvsp[(3) - (3)]));
				(yyval) = (yyvsp[(1) - (3)]);
			}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 592 "1605058.y"
    {
 		  		(yyvsp[(3) - (6)])->size = atoi((yyvsp[(5) - (6)])->name.c_str());
				(yyvsp[(1) - (6)])->alist.push_back((yyvsp[(3) - (6)]));
				(yyval) = (yyvsp[(1) - (6)]);
 		  }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 598 "1605058.y"
    {
 		  		SymbolInfo* s = new SymbolInfo();
 				s->alist.push_back((yyvsp[(1) - (1)]));
 				(yyval) = s;
 		  }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 604 "1605058.y"
    {
 		  		(yyvsp[(1) - (4)])->size = atoi((yyvsp[(3) - (4)])->name.c_str());
				SymbolInfo* s = new SymbolInfo();
 				s->alist.push_back((yyvsp[(1) - (4)]));
 				(yyval) = s;
 		  }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 613 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
			//cout << "\n{\n" << $1->description << "\n}\n" << line_count;
			
		}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 619 "1605058.y"
    {
	   		(yyvsp[(1) - (2)])->description = (yyvsp[(1) - (2)])->description + "\n" + (yyvsp[(2) - (2)])->description;
	   		(yyval) = (yyvsp[(1) - (2)]);
	   		//cout << "\n{\n" << $1->description << "\n}\n"; 
	   }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 627 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 631 "1605058.y"
    {
	  		(yyval) = (yyvsp[(1) - (1)]);
	  	}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 635 "1605058.y"
    {
	  		(yyval) == (yyvsp[(1) - (1)]);
	  	}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 639 "1605058.y"
    {
	  		SymbolInfo* s = new SymbolInfo();
	  		string start = newLabel();
	  		string end = newLabel();
	  		s->description = ";For loop\n" + (yyvsp[(3) - (7)])->description + start + ":\n";
	  		s->description = s->description + (yyvsp[(4) - (7)])->description +"cmp " + (yyvsp[(4) - (7)])->tag + ", 0\n";
	  		s->description = s->description + "je " + end + "\n";
	  		s->description = s->description + (yyvsp[(7) - (7)])->description;
	  		s->description = s->description + (yyvsp[(5) - (7)])->description;
	  		s->description = s->description + "jmp " + start + "\n";
	  		s->description = s->description + end + ":\n";
	  		(yyval) = s;
	  	}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 653 "1605058.y"
    {
	  		SymbolInfo* s = new SymbolInfo();
	  		string end = newLabel();
	  		s->description = ";If statement\n" + (yyvsp[(3) - (5)])->description + "cmp " + (yyvsp[(3) - (5)])->tag + ", 0\n" + "je " + end + "\n";
	  		s->description = s->description + (yyvsp[(5) - (5)])->description;
	  		s->description = s->description + end + ":\n";
	  		(yyval) = s;
	  	}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 662 "1605058.y"
    {
	  		SymbolInfo* s = new SymbolInfo();
	  		string elseif = newLabel();
	  		string end = newLabel();
	  		s->description = ";If-else statement\n" + (yyvsp[(3) - (7)])->description + "cmp " + (yyvsp[(3) - (7)])->tag + ", 0\n" + "je " + elseif + "\n";
	  		s->description = s->description + (yyvsp[(5) - (7)])->description;
	  		s->description = s->description + "jmp " + end + "\n";
	  		s->description = s->description + elseif + ":\n";
	  		s->description = s->description + (yyvsp[(7) - (7)])->description;
	  		s->description = s->description + end + ":\n";
	  		(yyval) = s;
	  	}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 675 "1605058.y"
    {
	  		SymbolInfo* s = new SymbolInfo();
	  		string start = newLabel();
	  		string end = newLabel();
	  		s->description = ";While loop\n" + start + ":\n";
	  		s->description = s->description + "cmp " + (yyvsp[(3) - (5)])->tag + ", 0\n";
	  		s->description = s->description + "je " + end + "\n";
	  		s->description = s->description + (yyvsp[(5) - (5)])->description + (yyvsp[(3) - (5)])->description;
	  		s->description = s->description + "jmp " + start + "\n";
	  		s->description = s->description + end + ":\n";
	  		(yyval) = s;
	  	}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 688 "1605058.y"
    {
	  		SymbolInfo* s = new SymbolInfo();
	  		SymbolInfo* ss = table->lookUp((yyvsp[(3) - (5)])->name);
	  		s->description = "mov ax, " + ss->tag + "\ncall println\n";
	  		(yyval) = s;
	  	}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 695 "1605058.y"
    {
	  		if(currentFunc == "main")
	  		{
	  			(yyvsp[(2) - (3)])->description = (yyvsp[(2) - (3)])->description + "\nmov ah, 4ch\nint 21h\n";
	  		}
	  		else
	  		{
	  			(yyvsp[(2) - (3)])->description = (yyvsp[(2) - (3)])->description + mov("dx", (yyvsp[(2) - (3)])) + "ret\n";
	  		}
	  		(yyval) = (yyvsp[(2) - (3)]);
	  		if((yyvsp[(2) - (3)])->type != returnType)
	  		{
	  			fprintf(logout, "Error at Line %d: %s return type expected\n", line_count, returnType.c_str());
	  			error++;
	  		}
	  	}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 714 "1605058.y"
    {
			(yyval) = new SymbolInfo();
			(yyval)->description = "";
		}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 719 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (2)]);
		}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 725 "1605058.y"
    {
			SymbolInfo* s = table->lookUp((yyvsp[(1) - (1)])->name);
			if(s)
			{
				(yyvsp[(1) - (1)])->type = s->type;
				(yyvsp[(1) - (1)])->size = s->size;
				(yyvsp[(1) - (1)])->tag = s->tag;
			}
			else
			{
				fprintf(logout, "Error at Line %d: %s variable not declared\n", line_count, (yyvsp[(1) - (1)])->name.c_str());
				error++;
			}
			(yyval) =  (yyvsp[(1) - (1)]);
		}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 741 "1605058.y"
    {
	 		SymbolInfo* s = table->lookUp((yyvsp[(1) - (4)])->name);
			if(s)
			{
				(yyvsp[(1) - (4)])->type = s->type;
				(yyvsp[(1) - (4)])->tag = s->tag;
				if((yyvsp[(3) - (4)])->type != "CONST_INT")
				{
					fprintf(logout, "Error at Line %d: Non-integer Array Index\n", line_count);
					error++;
				}
			}
			else
			{
				fprintf(logout, "Error at Line %d: %s variable not declared\n", line_count, (yyvsp[(1) - (4)])->name.c_str());
				error++;
			}
			(yyvsp[(1) - (4)])->description = (yyvsp[(3) - (4)])->description + (yyvsp[(1) - (4)])->description;
	 		(yyval) = (yyvsp[(1) - (4)]);
	 		(yyval)->alist.push_back((yyvsp[(3) - (4)]));
	 	}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 765 "1605058.y"
    {
 			string poping1 = "";
			if((yyvsp[(1) - (1)])->size == -100)
			{
					poping1 = "pop " + (yyvsp[(1) - (1)])->tag + "\n";
					(yyvsp[(1) - (1)])->size = 0;
			}
			(yyvsp[(1) - (1)])->description = (yyvsp[(1) - (1)])->description + poping1;
 			(yyval) = (yyvsp[(1) - (1)]);
 		}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 776 "1605058.y"
    {
	   		if((yyvsp[(1) - (3)])->type == "" || (yyvsp[(3) - (3)])->type == ""){}
	   		else if ((yyvsp[(1) - (3)])->type != (yyvsp[(3) - (3)])->type)
	   		{
	   			fprintf(logout, "Error at Line %d: Type Mismatch\n", line_count);
	   			error++;
	   			//if($1->size == 0 && $3->size == 0) fprintf(logout, "Resolve: value %s auto type casted to %s\n", $3->name.c_str(), $1->type.c_str());
	   		}
	   		if((yyvsp[(1) - (3)])->size !=0 || (yyvsp[(3) - (3)])->size != 0)
	   		{
	   			
	   			fprintf(logout, "Error at Line %d: Type Mismatch\n", line_count);
	   			error++;
	   		}
	   		(yyvsp[(1) - (3)])->description = (yyvsp[(1) - (3)])->description + (yyvsp[(3) - (3)])->description;
	   		string poping1 = "";
			if((yyvsp[(3) - (3)])->size == -100)
			{
					poping1 = "pop " + (yyvsp[(3) - (3)])->tag + "\n";
					(yyvsp[(3) - (3)])->size = 0;
			}
	   		if((yyvsp[(1) - (3)])->alist.size() != 0)
			{
				(yyvsp[(1) - (3)])->description = (yyvsp[(1) - (3)])->description + ";Array handling\n" + mov("si", (yyvsp[(1) - (3)])->alist[0]) + "shl si, 1\n" + poping1;
				(yyvsp[(1) - (3)])->description = (yyvsp[(1) - (3)])->description + mov("bx", (yyvsp[(3) - (3)])) + "mov " + (yyvsp[(1) - (3)])->tag + "[si], bx\n"; 
			}
			else
			{
				(yyvsp[(1) - (3)])->description = (yyvsp[(1) - (3)])->description + poping1 + mov("bx", (yyvsp[(3) - (3)])) + "mov " + (yyvsp[(1) - (3)])->tag + ", bx\n"; 
			}
	   }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 810 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 814 "1605058.y"
    {
			string poping1 = "";
			string poping2 = "";
			if((yyvsp[(1) - (3)])->size == -100)
			{
					poping1 = "pop " + (yyvsp[(1) - (3)])->tag + "\n";
					(yyvsp[(1) - (3)])->size = 0;
			}
			if((yyvsp[(3) - (3)])->size == -100)
			{
					poping2 = "pop " + (yyvsp[(3) - (3)])->tag + "\n";
					(yyvsp[(3) - (3)])->size = 0;
			}
			(yyval) = new SymbolInfo("","");
			(yyval)->tag = newTempInsert();
			(yyval)->description = (yyvsp[(1) - (3)])->description + (yyvsp[(3) - (3)])->description + poping1 + poping2;
			if((yyvsp[(2) - (3)])->name == "||")
			{
				string task = newLabel();
				string endIf = newLabel();
				(yyval)->description = (yyval)->description + "cmp " + (yyvsp[(1) - (3)])->tag + ", 0\n";
				(yyval)->description = (yyval)->description + "jne " + task + "\n";
				(yyval)->description = (yyval)->description + "cmp " + (yyvsp[(3) - (3)])->tag + ", 0\n";
				(yyval)->description = (yyval)->description + "jne " + task + "\n";
				(yyval)->description = (yyval)->description + "jmp " + endIf + "\n";
				(yyval)->description = (yyval)->description + task +":\n";
				(yyval)->description = (yyval)->description + "mov " + (yyval)->tag + ", 1\n";
				(yyval)->description = (yyval)->description + endIf + ":\n";
			}
			else if((yyvsp[(2) - (3)])->name == "&&")
			{
				string endIf = newLabel();
				(yyval)->description = (yyval)->description + "cmp " + (yyvsp[(1) - (3)])->tag + ", 0\n";
				(yyval)->description = (yyval)->description + "je " + endIf + "\n";
				(yyval)->description = (yyval)->description + "cmp " + (yyvsp[(3) - (3)])->tag + ", 0\n";
				(yyval)->description = (yyval)->description + "je " + endIf + "\n";
				(yyval)->description = (yyval)->description + "mov " + (yyval)->tag + ", 1\n";
				(yyval)->description = (yyval)->description + endIf + ":\n";
			}
		}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 857 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 861 "1605058.y"
    {
			string poping1 = "";
			string poping2 = "";
			if((yyvsp[(1) - (3)])->size == -100)
			{
					poping1 = "pop " + (yyvsp[(1) - (3)])->tag + "\n";
					(yyvsp[(1) - (3)])->size = 0;
			}
			if((yyvsp[(3) - (3)])->size == -100)
			{
					poping2 = "pop " + (yyvsp[(3) - (3)])->tag + "\n";
					(yyvsp[(3) - (3)])->size = 0;
			}
			(yyval) = new SymbolInfo();
			(yyval)->tag = newTempInsert();
			(yyval)->description = (yyvsp[(1) - (3)])->description + (yyvsp[(3) - (3)])->description + poping1 + poping2;
			if((yyvsp[(2) - (3)])->name == ">")
			{
				(yyval)->description = (yyval)->description +  "mov " + (yyval)->tag + ", " + "0\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (3)])->tag + "\n";
				(yyval)->description = (yyval)->description + "cmp " + "bx, " + (yyvsp[(3) - (3)])->tag + "\n";
				string label = newLabel();
				(yyval)->description = (yyval)->description + "jng " + label + "\n" + "mov " + (yyval)->tag + ", " + "1\n";
				(yyval)->description = (yyval)->description + label + ":\n";

			}
			else if((yyvsp[(2) - (3)])->name == "<")
			{
				(yyval)->description = (yyval)->description +  "mov " + (yyval)->tag + ", " + "0\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (3)])->tag + "\n";
				(yyval)->description = (yyval)->description + "cmp " + "bx, " + (yyvsp[(3) - (3)])->tag + "\n";
				string label = newLabel();
				(yyval)->description = (yyval)->description + "jnl " + label + "\n" + "mov " + (yyval)->tag + ", " + "1\n";
				(yyval)->description = (yyval)->description + label + ":\n";
			}
			else if((yyvsp[(2) - (3)])->name == ">=")
			{
				(yyval)->description = (yyval)->description +  "mov " + (yyval)->tag + ", " + "0\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (3)])->tag + "\n";
				(yyval)->description = (yyval)->description + "cmp " + "bx, " + (yyvsp[(3) - (3)])->tag + "\n";
				string label = newLabel();
				(yyval)->description = (yyval)->description + "jl " + label + "\n" + "mov " + (yyval)->tag + ", " + "1\n";
				(yyval)->description = (yyval)->description + label + ":\n";
			} 
			else if((yyvsp[(2) - (3)])->name == "<=")
			{
				(yyval)->description = (yyval)->description +  "mov " + (yyval)->tag + ", " + "0\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (3)])->tag + "\n";
				(yyval)->description = (yyval)->description + "cmp " + "bx, " + (yyvsp[(3) - (3)])->tag + "\n";
				string label = newLabel();
				(yyval)->description = (yyval)->description + "jg " + label + "\n" + "mov " + (yyval)->tag + ", " + "1\n";
				(yyval)->description = (yyval)->description + label + ":\n";
			} 
			else if((yyvsp[(2) - (3)])->name == "==")
			{
				(yyval)->description = (yyval)->description +  "mov " + (yyval)->tag + ", " + "0\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (3)])->tag + "\n";
				(yyval)->description = (yyval)->description + "cmp " + "bx, " + (yyvsp[(3) - (3)])->tag + "\n";
				string label = newLabel();
				(yyval)->description = (yyval)->description + "jne " + label + "\n" + "mov " + (yyval)->tag + ", " + "1\n";
				(yyval)->description = (yyval)->description + label + ":\n";
			} 
			else if((yyvsp[(2) - (3)])->name == "!=")
			{
				(yyval)->description = (yyval)->description +  "mov " + (yyval)->tag + ", " + "0\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (3)])->tag + "\n";
				(yyval)->description = (yyval)->description + "cmp " + "bx, " + (yyvsp[(3) - (3)])->tag + "\n";
				string label = newLabel();
				(yyval)->description = (yyval)->description + "je " + label + "\n" + "mov " + (yyval)->tag + ", " + "1\n";
				(yyval)->description = (yyval)->description + label + ":\n";
			} 
			(yyval)->type = "CONST_INT";
		}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 937 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 941 "1605058.y"
    {
			SymbolInfo* s = new SymbolInfo("", "");
			s->tag = newTempInsert();
			string poping1 = "";
			string poping2 = "";
			if((yyvsp[(1) - (3)])->size == -100)
			{
					poping1 = "pop " + (yyvsp[(1) - (3)])->tag + "\n";
					(yyvsp[(1) - (3)])->size = 0;
			}
			if((yyvsp[(3) - (3)])->size == -100)
			{
					poping2 = "pop " + (yyvsp[(3) - (3)])->tag + "\n";
					(yyvsp[(3) - (3)])->size = 0;
			}
			if((yyvsp[(2) - (3)])->name == "+") s->description = (yyvsp[(3) - (3)])->description + (yyvsp[(1) - (3)])->description + poping1 + poping2 + mov("bx", (yyvsp[(1) - (3)])) + mov("cx", (yyvsp[(3) - (3)])) + "add bx, cx\n";
			else s->description = (yyvsp[(3) - (3)])->description + (yyvsp[(1) - (3)])->description + mov("bx", (yyvsp[(1) - (3)])) + mov("cx", (yyvsp[(3) - (3)])) + "sub bx, cx\n";
			s->description = s->description + mov(s, "bx");
			(yyval) = s;
		}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 964 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 968 "1605058.y"
    {
     		(yyval) = new SymbolInfo("","");
     		(yyval)->tag = newTempInsert();
     		string poping1 = "";
			string poping2 = "";
			if((yyvsp[(1) - (3)])->size == -100)
			{
					poping1 = "pop " + (yyvsp[(1) - (3)])->tag + "\n";
					(yyvsp[(1) - (3)])->size = 0;
			}
			if((yyvsp[(3) - (3)])->size == -100)
			{
					poping2 = "pop " + (yyvsp[(3) - (3)])->tag + "\n";
					(yyvsp[(3) - (3)])->size = 0;
			}
     		(yyval)->description = (yyvsp[(3) - (3)])->description + (yyvsp[(1) - (3)])->description + poping1 + poping2;

     		if((yyvsp[(3) - (3)])->type != "CONST_INT" || (yyvsp[(1) - (3)])->type != "CONST_INT")
     		{
     			fprintf(logout, "Error at Line %d: Non-integer operand on modulus operator\n", line_count);
     		}
     		if((yyvsp[(2) - (3)])->name == "*")
     		{
     			(yyval)->description = (yyval)->description + mov("ax", (yyvsp[(1) - (3)])) + mov("bx", (yyvsp[(3) - (3)])) + "imul bx\n" + mov((yyval), "ax");
     		}
     		else if((yyvsp[(2) - (3)])->name == "%")
     		{
     			(yyval)->description = (yyval)->description + "mov dx, 0\n" + mov("ax", (yyvsp[(1) - (3)])) + mov("bx", (yyvsp[(3) - (3)])) + "idiv bx\n" + mov((yyval), "dx");
     		}
     		else
     		{
     			(yyval)->description = (yyval)->description + "mov dx, 0\n" + mov("ax", (yyvsp[(1) - (3)])) + mov("bx", (yyvsp[(3) - (3)])) + "idiv bx\n" + mov((yyval), "ax");
     		}
     	}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 1005 "1605058.y"
    {
			(yyval) = (yyvsp[(2) - (2)]);
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 1009 "1605058.y"
    {
			(yyvsp[(2) - (2)])->description = ";NOT unary_expression\n";
			(yyval) = (yyvsp[(2) - (2)]);
		}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 1014 "1605058.y"
    {
			if((yyvsp[(1) - (1)])->type == "VOID")
			{
				fprintf(logout, "Error at Line %d: Void is not allowed here\n", line_count);
			}
			if((yyvsp[(1) - (1)])->alist.size() != 0)
			{
				(yyval) = new SymbolInfo("", "");
				(yyval)->tag = newTempInsert();
				(yyval)->description = (yyvsp[(1) - (1)])->description + ";Array-handling\n" + mov("si", (yyvsp[(1) - (1)])->alist[0]) + "shl si, 1\n";
				(yyval)->description = (yyval)->description + "mov bx, " + (yyvsp[(1) - (1)])->tag + "[si]\n";
				(yyval)->description = (yyval)->description + mov((yyval), "bx");
			}
			else
			{
				(yyval) = (yyvsp[(1) - (1)]);
			}
		}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 1035 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 1039 "1605058.y"
    {
			SymbolInfo* s = table->lookUp((yyvsp[(1) - (4)])->name);
			SymbolInfo* temp = new SymbolInfo("","");
			if(s)
			{
				(yyvsp[(1) - (4)])->type = s->type;
				if(s->size >= 0)
				{
					fprintf(logout, "Error at Line %d: %s is not a function\n", line_count, (yyvsp[(1) - (4)])->name.c_str());
				}
				else if(s->alist.size() > (yyvsp[(3) - (4)])->alist.size())
				{
					fprintf(logout, "Error at Line %d: Missing parameters in %s function call\n", line_count, (yyvsp[(1) - (4)])->name.c_str());
				}
				else if(s->alist.size() < (yyvsp[(3) - (4)])->alist.size())
				{
					fprintf(logout, "Error at Line %d: Too much parameters in %s function call\n", line_count, (yyvsp[(1) - (4)])->name.c_str());
				}
				else
				{
					int i = 0;
					for(i=0; i<s->alist.size(); i++)
					{
						if(s->alist[i]->type != (yyvsp[(3) - (4)])->alist[i]->type) break;
					}
					if(i<s->alist.size()) fprintf(logout, "Error at Line %d: Parameter type mismatch in %s function call\n", line_count, (yyvsp[(1) - (4)])->name.c_str());
				}
				temp->description = temp->description + backup(table->lookUp(currentFunc));
				for(int i=0; i<s->alist.size(); i++)
				{
					temp->description = temp->description + (yyvsp[(3) - (4)])->alist[i]->description;
				}
				for(int i=0; i<s->alist.size(); i++)
				{
					temp->description = temp->description + mov("bx", (yyvsp[(3) - (4)])->alist[i]) + mov(s->alist[i], "bx");
				}
				temp->tag = newTempInsert();
				temp->description = temp->description + "call " + s->name + "\n" + mov(temp, "dx");
				temp->description = temp->description + restore(table->lookUp(currentFunc)) + "push " + temp->tag + "\n";
				temp->size = -100; 
			}
			else
			{
				fprintf(logout, "Error at Line %d: %s function not declared\n", line_count, (yyvsp[(1) - (4)])->name.c_str());
			}
			(yyval) =  temp;
		}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 1087 "1605058.y"
    {
			(yyval) = (yyvsp[(2) - (3)]);
		}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1091 "1605058.y"
    {
			(yyvsp[(1) - (1)])->tag = newTempInsert();
			(yyvsp[(1) - (1)])->description = mov((yyvsp[(1) - (1)]), (yyvsp[(1) - (1)])->name);
			(yyval) =(yyvsp[(1) - (1)]);
		}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1097 "1605058.y"
    {
			(yyvsp[(1) - (1)])->tag = newTempInsert();
			(yyvsp[(1) - (1)])->description = mov((yyvsp[(1) - (1)]), (yyvsp[(1) - (1)])->name);
			(yyval) =(yyvsp[(1) - (1)]);
		}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1103 "1605058.y"
    {
			if((yyvsp[(1) - (2)])->alist.size() != 0)
			{
				(yyvsp[(1) - (2)])->description = mov("si", (yyvsp[(1) - (2)])->alist[0]) + "shl si, 1\n" + "inc " + (yyvsp[(1) - (2)])->tag + "+si\n";
			}
			else
			{
				(yyvsp[(1) - (2)])->description = "inc " + (yyvsp[(1) - (2)])->tag + "\n";
			}
			(yyval) =(yyvsp[(1) - (2)]);
		}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1115 "1605058.y"
    {
			if((yyvsp[(1) - (2)])->alist.size() != 0)
			{
				(yyvsp[(1) - (2)])->description = mov("si", (yyvsp[(1) - (2)])->alist[0]) + "shl si, 1\n" + "dec " + (yyvsp[(1) - (2)])->tag + "+si\n";
			}
			else
			{
				(yyvsp[(1) - (2)])->description = "dec " + (yyvsp[(1) - (2)])->tag + "\n";
			}
			(yyval) =(yyvsp[(1) - (2)]);
		}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 1129 "1605058.y"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 1133 "1605058.y"
    {
			SymbolInfo* s = new SymbolInfo();
 			(yyval) = s;
		}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 1140 "1605058.y"
    {
			SymbolInfo* temp = new SymbolInfo("", "");
			temp->tag = newTempInsert();
			//$1->description = $1->description + $3->description;
			temp->description = (yyvsp[(3) - (3)])->description + mov("bx", (yyvsp[(3) - (3)])) + mov(temp, "bx");
			(yyvsp[(1) - (3)])->alist.push_back(temp);
			(yyval) = (yyvsp[(1) - (3)]);
		}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 1149 "1605058.y"
    {
	    	SymbolInfo* s = new SymbolInfo();
	    	SymbolInfo* temp = new SymbolInfo("", "");
			temp->tag = newTempInsert();
			temp->description = (yyvsp[(1) - (1)])->description + mov("bx", (yyvsp[(1) - (1)])) + mov(temp, "bx");
 			s->alist.push_back(temp);
 			//s->description = $1->description;
 			(yyval) = s;
	    }
    break;



/* Line 1455 of yacc.c  */
#line 2909 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1161 "1605058.y"

int main(int argc,char *argv[])
{

	functions.push_back(0);
	SymbolInfo* printFunction = new SymbolInfo("println", "");
	printFunction->description = "push ax\n\
push bx\n\
push cx\n\
push dx\n\
or ax, ax\n\
jge @end_if1\n\
push ax\n\
mov dl, '-'\n\
mov ah, 2\n\
int 21h\n\
pop ax\n\
neg ax\n\
@end_if1:\n\
xor cx, cx\n\
mov bx, 10D\n\
@repeat1:\n\
xor dx,dx\n\
div bx\n\
push dx\n\
inc cx\n\
or ax, ax\n\
jne @repeat1\n\
mov ah, 2\n\
@print_loop:\n\
pop dx\n\
or dl, 30h\n\
int 21h\n\
loop @print_loop\n\
mov dl, 0ah\n\
int 21h\n\
mov dl, 0dh\n\
int 21h\n\
pop dx\n\
pop cx\n\
pop bx\n\
pop ax\n\
ret\n";


	functions.push_back(printFunction);
	if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	codeout= fopen("code.asm","w");
	logout= fopen("log.txt","w");
	

	yyin=fp;
	yyparse();
	
	fprintf(logout, "\nLine count: %d\nError count: %d", line_count, error);

	fclose(codeout);
	fclose(logout);

	peephole();
	
	return 0;
}


