%{

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ojprotos.h"
#include "grammer.tab.h"


uint32_t ojproto_version;
char _structName[OJPLEN];
char _structType[OJPLEN];
char _varName[OJPLEN];
char _varType[OJPLEN];
char *output_file;
char *to_include;

%}

%union {
   uint32_t int4;
   char *str;
};

%token VERSION
%token <str> STRING
%token <int4> UINT32
%token LBRACE
%token RBRACE
%token LBRKT
%token RBRKT
%token <str> TYPE
%token STRUCT
%token PROTOBEGIN
%token PROTOEND
%token SEMI

%type <int4> array
%type <str> structname
%type <str> struct



%%

prog:	version PROTOBEGIN structs PROTOEND

version: VERSION UINT32
	 {
	    ojproto_version = (uint32_t) $2;
	    printf("Found version %d\n", $2);
	    prepare_file(output_file, to_include);
	 }

structs: struct
	| structs struct
	
struct: structname LBRACE members RBRACE SEMI
	{
	    add_struct($1);
	    print_last_struct(output_file);
	    init_members(); 
	}

members: member
	| members member
	
member: TYPE STRING array SEMI
	{
	    if(can_add_primitive_member($2) == OJPTRUE)
	    {
		struct SMemberTypes SType;
		strcpy(SType.name, $2);
		SType.isStruct = OJPFALSE;
		SType.isArray = OJPTRUE;
		SType.arraySize = $3;
		SType.eMemberType = type_to_str($1);
		memset(&SType.structName, 0, OJPLEN);
		add_member(&SType); 
	    }
	    else
	    {
		printf("Cannot add member[] %s %s\n", $1, $2);
	    }
	}
	| TYPE STRING SEMI
	{
	    if(can_add_primitive_member($2) == OJPTRUE)
	    {
		struct SMemberTypes SType;
		strcpy(SType.name, $2);
		SType.isStruct = OJPFALSE;
		SType.isArray = OJPFALSE;
		SType.arraySize = 0;
		SType.eMemberType = type_to_str($1);
		memset(&SType.structName, 0, OJPLEN);
		add_member(&SType); 
	    }
	    else
	    {
		printf("Cannot add member %s %s\n", $1, $2);
	    }
	}
	| structname STRING array SEMI
	{
	    strcpy(_structType, $1);
	    strcpy(_structName, $2);
	    if(can_add_struct_member($1) == OJPTRUE && can_add_primitive_member($2) == OJPTRUE)
	    {
	    printf("Adding struct %s %s\n", $1, $2);
		struct SMemberTypes SType;
		strcpy(SType.name, $2);
		strcpy(SType.structName, $1);
		SType.isStruct = OJPTRUE;
		SType.isArray = OJPTRUE;
		SType.arraySize = $3;
		SType.eMemberType = type_to_str($1);
		add_member(&SType);
	    }
	    else
	    {
		printf("Cannot add struct[] %s %s\n", $1, $2);
	    }
	}
	
	| structname STRING SEMI
	{
	    if(can_add_struct_member($1) == OJPTRUE && can_add_primitive_member($2) == OJPTRUE)
	    {
		struct SMemberTypes SType;
		strcpy(SType.structName, $1);
		strcpy(SType.name, $2);
		SType.isStruct = OJPTRUE;
		SType.isArray = OJPFALSE;
		SType.arraySize = 0;
		SType.eMemberType = type_to_str($1);
		add_member(&SType);
	    }
	    else
	    {
		printf("Cannot add struct %s %s\n", $1, $2);
	    }
	}
	
structname: STRUCT STRING
    {
	printf("Found struct: %s\n", $2);
	$$ = $2; 
    }
	
array: LBRKT UINT32 RBRKT
	{
	    $$ = (uint32_t) $2;
	}	   
	
%%
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

main(int argc, char *argv[])
{
  if( argc != 3)
  {
    printf("Error: Usage: ser proto outputfile\n");
    return 0;
  }
  init_all();
  output_file = argv[2];
  to_include = argv[1];
  yyin = fopen(argv[1], "r");
  printf("yyin is %d and argv1 is %s\n", yyin, argv[1]);
  yyparse();
  fclose(yyin); 
  
 // clean();
}

yyerror(char *s)
{
	fprintf(stderr, "error I found: %s at line %d\n", s, yylineno);
	fprintf(stderr, "error is %s\n", yytext);
} 
