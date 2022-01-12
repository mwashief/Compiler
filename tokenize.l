%{
#include<iostream>
#include<vector>
#include<stdlib.h>
#include<string.h>
#include<string>
#include "symboltable.cpp"
#include "y.tab.h"


using namespace std;

extern YYSTYPE yylval;
extern SymbolTable *table;
void yyerror(char *);

int line_count = 1;
int error = 0;
int scope = 0;
extern FILE *logout;


%}



digit [0-9]
letter [A-Za-z]
delim [ \t\r\f\v]
newline \n
ws [delim]+
id (_|{letter})(_|{letter}|{digit})*
integer {digit}+
number {digit}*(\.{digit}+)?(E[+-]?{digit}+)?
ADDOP [+-] 
MULOP [*/%]

%%

{delim}+	{}
{newline} {line_count++;}

"if" {return IF;}
"else" {return ELSE;}
"for" {return FOR;}
"while" {return WHILE;}
"int" {return INT;}
"float" {return FLOAT;}
"return" {return RETURN;}
"void" {return VOID;}
"println"	{return PRINTLN;}

{ADDOP} { 
			SymbolInfo *s= new  SymbolInfo(yytext, (char *)"ADDOP");
			s->description = s->name;
			yylval = (YYSTYPE)s;
			return ADDOP;
		}



{MULOP}	{
			SymbolInfo *s= new  SymbolInfo(yytext, (char *)"MULOP");
			s->description = s->name;
			if(s->name != "*") s->description = s->description + s->name;
			yylval = (YYSTYPE)s;
			return MULOP;
		}




"=" 	{return ASSIGNOP;}

">" |
"<" |
">=" |
"<=" |
"==" |
"!=" {
		SymbolInfo *s= new  SymbolInfo(yytext, (char *)"RELOP");
		s->description = s->name;
		yylval = (YYSTYPE)s;
		return RELOP;
	}

"&&"	|
"||"	{
			SymbolInfo *s= new  SymbolInfo(yytext, (char *)"LOGICOP");
			s->description = s->name;
			yylval = (YYSTYPE)s;
			return LOGICOP;
		}
"!"		{	return NOT; }


";" 	{ return SEMICOLON;}
"," 	{return COMMA;}
"(" 	{return LPAREN;}
")" 	{return RPAREN;}
"{" 	{
			scope++;
			return LCURL;
		}
"}" 	{
			if(scope>=1)
			{
				scope--;
				return RCURL;
			}
			fprintf(logout, "Error at line %d : Extra RCURL\n", line_count);
		}
"[" 	{return LTHIRD;}
"]" 	{return RTHIRD;}
"++" 	{return INCOP;}
"--" 	{return DECOP;}



{integer}	{
				SymbolInfo *s= new  SymbolInfo(yytext, (char *)"CONST_INT");
				s->description = s->name;
				yylval = (YYSTYPE)s;
				return CONST_INT;
			}
{number} 	{	
				SymbolInfo *s= new  SymbolInfo(yytext, (char *)"CONST_FLOAT");
				s->description = s->name;
				yylval = (YYSTYPE)s;
				return CONST_FLOAT; 
			}
{id} 		{	 
				
				SymbolInfo *s= new  SymbolInfo(yytext, (char *)"");
				s->description = "";
				yylval = (YYSTYPE)s;
							
				return ID;
			}


%%

int yywrap(void)
{
	return 1;
}
