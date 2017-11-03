 
%{
#include <stdio.h>
#include <stdint.h>
#include "grammer.tab.h"

%}

%option yylineno

%%
VERSION			return VERSION;
[0-9]+			{ 
			    yylval.int4 = (uint32_t) atoi(yytext); 
			    return UINT32; 
			}
"\{" 			return LBRACE;
"\}"			return RBRACE;
"\[" 			return LBRKT;
"\]"			return RBRKT;
;   			return SEMI;
:               return DOTCOLON; 
struct 			return STRUCT;
uint32_t		{ yylval.str = strdup(yytext); return TYPE; }
int32_t			{ yylval.str = strdup(yytext); return TYPE; }
uint16_t		{ yylval.str = strdup(yytext); return TYPE; }
int16_t			{ yylval.str = strdup(yytext); return TYPE; }
uint8_t			{ yylval.str = strdup(yytext); return TYPE; }
int8_t			{ yylval.str = strdup(yytext); return TYPE; }
char			{ yylval.str = strdup(yytext); return TYPE; }
OJPROTO			return PROTOBEGIN;
OJPROTOEND		return PROTOEND;
[a-zA-Z"][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return STRING; }
\n
. 		

%%
int yywrap()
{
	return 1;
}
