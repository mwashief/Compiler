%{

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

%}

%token ID SEMICOLON COMMA LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD INT FLOAT VOID CONST_INT CONST_FLOAT FOR IF ELSE WHILE PRINTLN RETURN ASSIGNOP LOGICOP INCOP DECOP RELOP ADDOP MULOP NOT

%left ADDOP MULOP RELOP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%start start


%%

start : program
	{
     	$$ =$1;
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
	;

program : program unit
	{
     	$1->description = $1->description + "\n" + $2->description + "\n";
     	$$ = $1;
     	
     }
	| unit
	 {
     	$$ = $1;
     }
	;
	
unit : var_declaration
 	{
     	$$ = $1;
     }
     | func_declaration
     {
     	$$ = $1;
     	params = 0;

     }
     | func_definition
     {
     	$$ = $1;
     }
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
		{
			SymbolInfo* s= new SymbolInfo($2->name, (char *)"ID");
			s->type = $1->type;
			s->alist = $4->alist;
			s->description = $1->description + " " + $2->description + "(" + $4->description + ");";
			$$ = s;
			if(table->lookUp($$->name))
			{
				fprintf(logout, "Error at Line %d: Multiple Declaration of function %s\n", line_count, $$->name.c_str());
				error++;
			}
			else
			{
				table->insert($$);
				$$->size = -1;
			}
		}
		| type_specifier ID LPAREN RPAREN SEMICOLON
		{
			SymbolInfo* s= new SymbolInfo($2->name, (char *)"ID");
			s->type = $1->type;
			s->description = $1->description + " " + $2->description + "();";
			$$ = s;
			if(table->lookUp($$->name))
			{
				fprintf(logout, "Error at Line %d: Multiple Declaration of function %s\n", line_count, $$->name.c_str());
				error++;
			}
			else
			{
				table->insert($$);
				$$->size = -1;
			}
		}
		;
		 
func_definition_head_one : type_specifier ID LPAREN parameter_list RPAREN
		{
			SymbolInfo* s= new SymbolInfo($2->name, (char *)"ID");
			s->type = $1->type;
			for(int i=0; i<$4->alist.size(); i++)
			{
				SymbolInfo* temp = new SymbolInfo($4->alist[i]->name, $4->alist[i]->type);
				temp->tag = $4->alist[i]->tag;
				s->alist.push_back(temp);
			}
			$$ = s;
			

			SymbolInfo* temp = table->lookUp($$->name);
			
			if(temp)
			{
				if(temp->size>=0)
				{
					fprintf(logout, "Error at Line %d: Already there is a variable with this name \"%s\"\n", line_count, $2->name.c_str());
					error++;
				}
				else
				{
					
					if (temp->type != $$->type)
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition return type mismatch\n", line_count, $2->name.c_str());
						error++;
					}
					if(temp->size == -2)
					{
						fprintf(logout, "Error at Line %d: Multiple defination of function %s\n", line_count, $$->name.c_str());
						error++;
					}
					if (temp->alist.size() != $$->alist.size())
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters differ in number\n", line_count, $2->name.c_str());
						error++;
					}
					int i;
					for(i=0; i<temp->alist.size(); i++)
					{
						if(temp->alist[i]->type != $$->alist[i]->type) break;
					}
					if(i == temp->alist.size())
					{
						table->insert($$);
						$$->size = -2;
					}
					else
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters mismatch\n", line_count, $2->name.c_str());
						error++;
					}
				}
			}
			else
			{
				table->insert($$);
				$$->size = -2;
			}

			returnType = $1->type;
			currentFunc = $2->name;
		}


func_definition_head_two : type_specifier ID LPAREN RPAREN
		{
			SymbolInfo* s= new SymbolInfo($2->name, (char *)"ID");
			s->type = $1->type;
			$$ = s;
			SymbolInfo* temp = table->lookUp($$->name);

			if(temp)
			{
				if(temp->size>=0)
				{
					fprintf(logout, "Error at Line %d: Already there is a variable with this name \"%s\"\n", line_count, $2->name.c_str());
					error++;
				}
				else
				{
					if (temp->type != $$->type)
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition return type mismatch\n", line_count, $2->name.c_str());
						error++;
					}
					if(temp->size == -2)
					{
						fprintf(logout, "Error at Line %d: Multiple defination of function %s\n", line_count, $$->name.c_str());
						error++;
					}
					if (temp->alist.size() != $$->alist.size())
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters differ in number\n", line_count, $2->name.c_str());
						error++;
					}
					
					int i;
					for(i=0; i<temp->alist.size(); i++)
					{
						if(temp->alist[i]->type != $$->alist[i]->type) break;
					}
					if(i == temp->alist.size())
					{
						table->insert($$);
						$$->size = -2;
					}
					else
					{
						fprintf(logout, "Error at Line %d: %s function declaration and definition parameters mismatch\n", line_count, $2->name.c_str());
						error++;
					}
				}
			}
			else
			{
				table->insert($$);
				$$->size = -2;
			}

			returnType = $1->type;
			currentFunc = $2->name;
		}
 		;


func_definition : func_definition_head_one compound_statement
			{
				$1->description = $2->description;
				$$ = $1;
     			returnType = "";
     			SymbolInfo* temp = new SymbolInfo($$->name, $$->type);
     			temp->description = $$->description;
     			if($$->name == "main")
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
			|
			func_definition_head_two compound_statement
			{
				$1->description = $2->description;
				$$ = $1;
     			returnType = "";
     			SymbolInfo* temp = new SymbolInfo($$->name, $$->type);
     			temp->description = $$->description;
     			if($$->name == "main")
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



parameter_list  : parameter_list COMMA type_specifier ID
		{
			$3->name = $4->name;
			$3->tag = newTempInsert();
			$1->alist.push_back($3);
			$$ = $1;
			params = $$;
		}
		| parameter_list COMMA type_specifier
		{
			$3->tag = newTempInsert();
			$1->alist.push_back($3);
			$$ = $1;
			params = $$;
		}
 		| type_specifier ID
 		{
 			$1->name = $2->name;
 			$1->tag = newTempInsert();
 			SymbolInfo* s = new SymbolInfo();
 			s->alist.push_back($1);
 			$$ = s;
 			params = $$;
 		}
		| type_specifier
		{
			SymbolInfo* s = new SymbolInfo();
 			s->alist.push_back($1);
 			$1->tag = newTempInsert();
 			$$ = s;
 			params = $$;
		}
 		;

 		
compound_statement : lcurl statements rcurl
			{
				$$ = $2;
			}
 		    | lcurl rcurl
 		    {
 		    	$$ = new SymbolInfo();
 		    	$$->description = "";
   		    }
 		    ;

lcurl : LCURL
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


rcurl : RCURL
			{
				table->exitScope();
			}
 		    
var_declaration : type_specifier declaration_list SEMICOLON
			{
				if($1->type != "VOID")
				{
					$2->type = $1->type;
					$$ = $2;
					for(int i=0; i<$$->alist.size(); i++)
					{
						$$->alist[i]->type = $1->type;
						if($$->alist[i]->name == "main")
						{
							fprintf(logout, "Error at Line %d: Variable name cannot be main\n", line_count);
							error++;
						}
						else if(table->current->lookUp($$->alist[i]->name))
						{
							fprintf(logout, "Error at Line %d: Multiple Declaration of %s\n", line_count, $$->alist[i]->name.c_str());
							error++;
						}
						else
						{
							$$->alist[i]->tag = newTemp();
							table->insert($$->alist[i]);
							SymbolInfo* temp = new SymbolInfo($$->alist[i]->name, $$->alist[i]->type);
							temp->tag = $$->alist[i]->tag;
							temp->size = $$->alist[i]->size;
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
 		 	;
 		 
type_specifier	: INT
			 { 
			 	SymbolInfo* s = new SymbolInfo((char *)"",(char *)"CONST_INT"); s->description = "int"; $$ = s;
			 }
 			| FLOAT 
 			{ 
 				SymbolInfo* s = new SymbolInfo((char *)"",(char *)"CONST_FLOAT"); s->description = "float"; $$ = s;
 			}
 			| VOID 
 			{ 
 				SymbolInfo* s = new SymbolInfo((char *)"",(char *)"VOID"); s->description = "void"; $$ = s;
 			}
 			;
 		
declaration_list : declaration_list COMMA ID
			{
				$1->alist.push_back($3);
				$$ = $1;
			}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD
 		  {
 		  		$3->size = atoi($5->name.c_str());
				$1->alist.push_back($3);
				$$ = $1;
 		  }
 		  | ID
 		  {
 		  		SymbolInfo* s = new SymbolInfo();
 				s->alist.push_back($1);
 				$$ = s;
 		  }
 		  | ID LTHIRD CONST_INT RTHIRD
 		  {
 		  		$1->size = atoi($3->name.c_str());
				SymbolInfo* s = new SymbolInfo();
 				s->alist.push_back($1);
 				$$ = s;
 		  }
 		  ;
 		  
statements : statement
		{
			$$ = $1;
			//cout << "\n{\n" << $1->description << "\n}\n" << line_count;
			
		}
	   | statements statement
	   {
	   		$1->description = $1->description + "\n" + $2->description;
	   		$$ = $1;
	   		//cout << "\n{\n" << $1->description << "\n}\n"; 
	   }
	   ;
	   
statement : var_declaration
		{
			$$ = $1;
		}
	  	| expression_statement
	  	{
	  		$$ = $1;
	  	}
	  	| compound_statement
	  	{
	  		$$ == $1;
	  	}
	  	| FOR LPAREN expression_statement expression_statement expression RPAREN statement
	  	{
	  		SymbolInfo* s = new SymbolInfo();
	  		string start = newLabel();
	  		string end = newLabel();
	  		s->description = ";For loop\n" + $3->description + start + ":\n";
	  		s->description = s->description + $4->description +"cmp " + $4->tag + ", 0\n";
	  		s->description = s->description + "je " + end + "\n";
	  		s->description = s->description + $7->description;
	  		s->description = s->description + $5->description;
	  		s->description = s->description + "jmp " + start + "\n";
	  		s->description = s->description + end + ":\n";
	  		$$ = s;
	  	}
	  	| IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
	  	{
	  		SymbolInfo* s = new SymbolInfo();
	  		string end = newLabel();
	  		s->description = ";If statement\n" + $3->description + "cmp " + $3->tag + ", 0\n" + "je " + end + "\n";
	  		s->description = s->description + $5->description;
	  		s->description = s->description + end + ":\n";
	  		$$ = s;
	  	}
	  	| IF LPAREN expression RPAREN statement ELSE statement
	  	{
	  		SymbolInfo* s = new SymbolInfo();
	  		string elseif = newLabel();
	  		string end = newLabel();
	  		s->description = ";If-else statement\n" + $3->description + "cmp " + $3->tag + ", 0\n" + "je " + elseif + "\n";
	  		s->description = s->description + $5->description;
	  		s->description = s->description + "jmp " + end + "\n";
	  		s->description = s->description + elseif + ":\n";
	  		s->description = s->description + $7->description;
	  		s->description = s->description + end + ":\n";
	  		$$ = s;
	  	}
	  	| WHILE LPAREN expression RPAREN statement
	  	{
	  		SymbolInfo* s = new SymbolInfo();
	  		string start = newLabel();
	  		string end = newLabel();
	  		s->description = ";While loop\n" + start + ":\n";
	  		s->description = s->description + "cmp " + $3->tag + ", 0\n";
	  		s->description = s->description + "je " + end + "\n";
	  		s->description = s->description + $5->description + $3->description;
	  		s->description = s->description + "jmp " + start + "\n";
	  		s->description = s->description + end + ":\n";
	  		$$ = s;
	  	}
	  	| PRINTLN LPAREN ID RPAREN SEMICOLON
	  	{
	  		SymbolInfo* s = new SymbolInfo();
	  		SymbolInfo* ss = table->lookUp($3->name);
	  		s->description = "mov ax, " + ss->tag + "\ncall println\n";
	  		$$ = s;
	  	}
	  	| RETURN expression SEMICOLON
	  	{
	  		if(currentFunc == "main")
	  		{
	  			$2->description = $2->description + "\nmov ah, 4ch\nint 21h\n";
	  		}
	  		else
	  		{
	  			$2->description = $2->description + mov("dx", $2) + "ret\n";
	  		}
	  		$$ = $2;
	  		if($2->type != returnType)
	  		{
	  			fprintf(logout, "Error at Line %d: %s return type expected\n", line_count, returnType.c_str());
	  			error++;
	  		}
	  	}
	  ;
	  
expression_statement 	: SEMICOLON	
		{
			$$ = new SymbolInfo();
			$$->description = "";
		}		
		| expression SEMICOLON 
		{
			$$ = $1;
		}
		;
	  
variable : ID
		{
			SymbolInfo* s = table->lookUp($1->name);
			if(s)
			{
				$1->type = s->type;
				$1->size = s->size;
				$1->tag = s->tag;
			}
			else
			{
				fprintf(logout, "Error at Line %d: %s variable not declared\n", line_count, $1->name.c_str());
				error++;
			}
			$$ =  $1;
		}	
	 	| ID LTHIRD expression RTHIRD
	 	{
	 		SymbolInfo* s = table->lookUp($1->name);
			if(s)
			{
				$1->type = s->type;
				$1->tag = s->tag;
				if($3->type != "CONST_INT")
				{
					fprintf(logout, "Error at Line %d: Non-integer Array Index\n", line_count);
					error++;
				}
			}
			else
			{
				fprintf(logout, "Error at Line %d: %s variable not declared\n", line_count, $1->name.c_str());
				error++;
			}
			$1->description = $3->description + $1->description;
	 		$$ = $1;
	 		$$->alist.push_back($3);
	 	}
	 	;
	 
 expression : logic_expression
 		{
 			string poping1 = "";
			if($1->size == -100)
			{
					poping1 = "pop " + $1->tag + "\n";
					$1->size = 0;
			}
			$1->description = $1->description + poping1;
 			$$ = $1;
 		}
	   | variable ASSIGNOP logic_expression
	   {
	   		if($1->type == "" || $3->type == ""){}
	   		else if ($1->type != $3->type)
	   		{
	   			fprintf(logout, "Error at Line %d: Type Mismatch\n", line_count);
	   			error++;
	   			//if($1->size == 0 && $3->size == 0) fprintf(logout, "Resolve: value %s auto type casted to %s\n", $3->name.c_str(), $1->type.c_str());
	   		}
	   		if($1->size !=0 || $3->size != 0)
	   		{
	   			
	   			fprintf(logout, "Error at Line %d: Type Mismatch\n", line_count);
	   			error++;
	   		}
	   		$1->description = $1->description + $3->description;
	   		string poping1 = "";
			if($3->size == -100)
			{
					poping1 = "pop " + $3->tag + "\n";
					$3->size = 0;
			}
	   		if($1->alist.size() != 0)
			{
				$1->description = $1->description + ";Array handling\n" + mov("si", $1->alist[0]) + "shl si, 1\n" + poping1;
				$1->description = $1->description + mov("bx", $3) + "mov " + $1->tag + "[si], bx\n"; 
			}
			else
			{
				$1->description = $1->description + poping1 + mov("bx", $3) + "mov " + $1->tag + ", bx\n"; 
			}
	   }	
	   ;
			
logic_expression : rel_expression
		{
			$$ = $1;
		}	
		| rel_expression LOGICOP rel_expression 
		{
			string poping1 = "";
			string poping2 = "";
			if($1->size == -100)
			{
					poping1 = "pop " + $1->tag + "\n";
					$1->size = 0;
			}
			if($3->size == -100)
			{
					poping2 = "pop " + $3->tag + "\n";
					$3->size = 0;
			}
			$$ = new SymbolInfo("","");
			$$->tag = newTempInsert();
			$$->description = $1->description + $3->description + poping1 + poping2;
			if($2->name == "||")
			{
				string task = newLabel();
				string endIf = newLabel();
				$$->description = $$->description + "cmp " + $1->tag + ", 0\n";
				$$->description = $$->description + "jne " + task + "\n";
				$$->description = $$->description + "cmp " + $3->tag + ", 0\n";
				$$->description = $$->description + "jne " + task + "\n";
				$$->description = $$->description + "jmp " + endIf + "\n";
				$$->description = $$->description + task +":\n";
				$$->description = $$->description + "mov " + $$->tag + ", 1\n";
				$$->description = $$->description + endIf + ":\n";
			}
			else if($2->name == "&&")
			{
				string endIf = newLabel();
				$$->description = $$->description + "cmp " + $1->tag + ", 0\n";
				$$->description = $$->description + "je " + endIf + "\n";
				$$->description = $$->description + "cmp " + $3->tag + ", 0\n";
				$$->description = $$->description + "je " + endIf + "\n";
				$$->description = $$->description + "mov " + $$->tag + ", 1\n";
				$$->description = $$->description + endIf + ":\n";
			}
		}	
		;
			
rel_expression	: simple_expression 
		{
			$$ = $1;
		}
		| simple_expression RELOP simple_expression	
		{
			string poping1 = "";
			string poping2 = "";
			if($1->size == -100)
			{
					poping1 = "pop " + $1->tag + "\n";
					$1->size = 0;
			}
			if($3->size == -100)
			{
					poping2 = "pop " + $3->tag + "\n";
					$3->size = 0;
			}
			$$ = new SymbolInfo();
			$$->tag = newTempInsert();
			$$->description = $1->description + $3->description + poping1 + poping2;
			if($2->name == ">")
			{
				$$->description = $$->description +  "mov " + $$->tag + ", " + "0\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "\n";
				$$->description = $$->description + "cmp " + "bx, " + $3->tag + "\n";
				string label = newLabel();
				$$->description = $$->description + "jng " + label + "\n" + "mov " + $$->tag + ", " + "1\n";
				$$->description = $$->description + label + ":\n";

			}
			else if($2->name == "<")
			{
				$$->description = $$->description +  "mov " + $$->tag + ", " + "0\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "\n";
				$$->description = $$->description + "cmp " + "bx, " + $3->tag + "\n";
				string label = newLabel();
				$$->description = $$->description + "jnl " + label + "\n" + "mov " + $$->tag + ", " + "1\n";
				$$->description = $$->description + label + ":\n";
			}
			else if($2->name == ">=")
			{
				$$->description = $$->description +  "mov " + $$->tag + ", " + "0\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "\n";
				$$->description = $$->description + "cmp " + "bx, " + $3->tag + "\n";
				string label = newLabel();
				$$->description = $$->description + "jl " + label + "\n" + "mov " + $$->tag + ", " + "1\n";
				$$->description = $$->description + label + ":\n";
			} 
			else if($2->name == "<=")
			{
				$$->description = $$->description +  "mov " + $$->tag + ", " + "0\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "\n";
				$$->description = $$->description + "cmp " + "bx, " + $3->tag + "\n";
				string label = newLabel();
				$$->description = $$->description + "jg " + label + "\n" + "mov " + $$->tag + ", " + "1\n";
				$$->description = $$->description + label + ":\n";
			} 
			else if($2->name == "==")
			{
				$$->description = $$->description +  "mov " + $$->tag + ", " + "0\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "\n";
				$$->description = $$->description + "cmp " + "bx, " + $3->tag + "\n";
				string label = newLabel();
				$$->description = $$->description + "jne " + label + "\n" + "mov " + $$->tag + ", " + "1\n";
				$$->description = $$->description + label + ":\n";
			} 
			else if($2->name == "!=")
			{
				$$->description = $$->description +  "mov " + $$->tag + ", " + "0\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "\n";
				$$->description = $$->description + "cmp " + "bx, " + $3->tag + "\n";
				string label = newLabel();
				$$->description = $$->description + "je " + label + "\n" + "mov " + $$->tag + ", " + "1\n";
				$$->description = $$->description + label + ":\n";
			} 
			$$->type = "CONST_INT";
		}
		;
				
simple_expression : term
		{
			$$ = $1;
		}
		| simple_expression ADDOP term 
		{
			SymbolInfo* s = new SymbolInfo("", "");
			s->tag = newTempInsert();
			string poping1 = "";
			string poping2 = "";
			if($1->size == -100)
			{
					poping1 = "pop " + $1->tag + "\n";
					$1->size = 0;
			}
			if($3->size == -100)
			{
					poping2 = "pop " + $3->tag + "\n";
					$3->size = 0;
			}
			if($2->name == "+") s->description = $3->description + $1->description + poping1 + poping2 + mov("bx", $1) + mov("cx", $3) + "add bx, cx\n";
			else s->description = $3->description + $1->description + mov("bx", $1) + mov("cx", $3) + "sub bx, cx\n";
			s->description = s->description + mov(s, "bx");
			$$ = s;
		}
		;
					
term :	unary_expression
		{
			$$ = $1;
		}
     	|  term MULOP unary_expression
     	{
     		$$ = new SymbolInfo("","");
     		$$->tag = newTempInsert();
     		string poping1 = "";
			string poping2 = "";
			if($1->size == -100)
			{
					poping1 = "pop " + $1->tag + "\n";
					$1->size = 0;
			}
			if($3->size == -100)
			{
					poping2 = "pop " + $3->tag + "\n";
					$3->size = 0;
			}
     		$$->description = $3->description + $1->description + poping1 + poping2;

     		if($3->type != "CONST_INT" || $1->type != "CONST_INT")
     		{
     			fprintf(logout, "Error at Line %d: Non-integer operand on modulus operator\n", line_count);
     		}
     		if($2->name == "*")
     		{
     			$$->description = $$->description + mov("ax", $1) + mov("bx", $3) + "imul bx\n" + mov($$, "ax");
     		}
     		else if($2->name == "%")
     		{
     			$$->description = $$->description + "mov dx, 0\n" + mov("ax", $1) + mov("bx", $3) + "idiv bx\n" + mov($$, "dx");
     		}
     		else
     		{
     			$$->description = $$->description + "mov dx, 0\n" + mov("ax", $1) + mov("bx", $3) + "idiv bx\n" + mov($$, "ax");
     		}
     	}
     	;

unary_expression : ADDOP unary_expression 
		{
			$$ = $2;
		}
		| NOT unary_expression
		{
			$2->description = ";NOT unary_expression\n";
			$$ = $2;
		}
		| factor
		{
			if($1->type == "VOID")
			{
				fprintf(logout, "Error at Line %d: Void is not allowed here\n", line_count);
			}
			if($1->alist.size() != 0)
			{
				$$ = new SymbolInfo("", "");
				$$->tag = newTempInsert();
				$$->description = $1->description + ";Array-handling\n" + mov("si", $1->alist[0]) + "shl si, 1\n";
				$$->description = $$->description + "mov bx, " + $1->tag + "[si]\n";
				$$->description = $$->description + mov($$, "bx");
			}
			else
			{
				$$ = $1;
			}
		}
		;
	
factor	: variable 
		{
			$$ = $1;
		}
		| ID LPAREN argument_list RPAREN
		{
			SymbolInfo* s = table->lookUp($1->name);
			SymbolInfo* temp = new SymbolInfo("","");
			if(s)
			{
				$1->type = s->type;
				if(s->size >= 0)
				{
					fprintf(logout, "Error at Line %d: %s is not a function\n", line_count, $1->name.c_str());
				}
				else if(s->alist.size() > $3->alist.size())
				{
					fprintf(logout, "Error at Line %d: Missing parameters in %s function call\n", line_count, $1->name.c_str());
				}
				else if(s->alist.size() < $3->alist.size())
				{
					fprintf(logout, "Error at Line %d: Too much parameters in %s function call\n", line_count, $1->name.c_str());
				}
				else
				{
					int i = 0;
					for(i=0; i<s->alist.size(); i++)
					{
						if(s->alist[i]->type != $3->alist[i]->type) break;
					}
					if(i<s->alist.size()) fprintf(logout, "Error at Line %d: Parameter type mismatch in %s function call\n", line_count, $1->name.c_str());
				}
				temp->description = temp->description + backup(table->lookUp(currentFunc));
				for(int i=0; i<s->alist.size(); i++)
				{
					temp->description = temp->description + $3->alist[i]->description;
				}
				for(int i=0; i<s->alist.size(); i++)
				{
					temp->description = temp->description + mov("bx", $3->alist[i]) + mov(s->alist[i], "bx");
				}
				temp->tag = newTempInsert();
				temp->description = temp->description + "call " + s->name + "\n" + mov(temp, "dx");
				temp->description = temp->description + restore(table->lookUp(currentFunc)) + "push " + temp->tag + "\n";
				temp->size = -100; 
			}
			else
			{
				fprintf(logout, "Error at Line %d: %s function not declared\n", line_count, $1->name.c_str());
			}
			$$ =  temp;
		}
		| LPAREN expression RPAREN
		{
			$$ = $2;
		}
		| CONST_INT
		{
			$1->tag = newTempInsert();
			$1->description = mov($1, $1->name);
			$$ =$1;
		}
		| CONST_FLOAT
		{
			$1->tag = newTempInsert();
			$1->description = mov($1, $1->name);
			$$ =$1;
		}
		| variable INCOP 
		{
			if($1->alist.size() != 0)
			{
				$1->description = mov("si", $1->alist[0]) + "shl si, 1\n" + "inc " + $1->tag + "+si\n";
			}
			else
			{
				$1->description = "inc " + $1->tag + "\n";
			}
			$$ =$1;
		}
		| variable DECOP
		{
			if($1->alist.size() != 0)
			{
				$1->description = mov("si", $1->alist[0]) + "shl si, 1\n" + "dec " + $1->tag + "+si\n";
			}
			else
			{
				$1->description = "dec " + $1->tag + "\n";
			}
			$$ =$1;
		}
	;
	
argument_list : arguments
		{
			$$ = $1;
		}
		|
		{
			SymbolInfo* s = new SymbolInfo();
 			$$ = s;
		}
		;
	
arguments : arguments COMMA logic_expression
		{
			SymbolInfo* temp = new SymbolInfo("", "");
			temp->tag = newTempInsert();
			//$1->description = $1->description + $3->description;
			temp->description = $3->description + mov("bx", $3) + mov(temp, "bx");
			$1->alist.push_back(temp);
			$$ = $1;
		}
	    | logic_expression
	    {
	    	SymbolInfo* s = new SymbolInfo();
	    	SymbolInfo* temp = new SymbolInfo("", "");
			temp->tag = newTempInsert();
			temp->description = $1->description + mov("bx", $1) + mov(temp, "bx");
 			s->alist.push_back(temp);
 			//s->description = $1->description;
 			$$ = s;
	    }
	    ;


%%
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

