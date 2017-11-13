 
%{
#include <stdio.h>
#include <stdint.h>
#include "grammer.tab.h"

int parse = 0;
%}

%option yylineno

%%
[0-9]+			{   if(parse == 1){
			    yylval.int4 = (uint32_t) atoi(yytext); 
			    return UINT32; 
			}}
"\{" 			{ if(parse == 1) return LBRACE; }
"\}"			{ if(parse == 1) return RBRACE; }
"\[" 			{ if(parse == 1) return LBRKT; }
"\]"			{ if (parse == 1) return RBRKT; }
;   			{ if(parse == 1) return SEMI; }
:                       { if(parse == 1) return DOTCOLON; }
struct 			{ if(parse == 1) return STRUCT; }
uint32_t		{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
int32_t			{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
uint16_t		{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
int16_t			{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
uint8_t			{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
int8_t			{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
char			{ if(parse == 1) { yylval.str = strdup(yytext); return TYPE; }}
"// OJPROTO"			{ parse = 1; return PROTOBEGIN; }
"// OJPROTOEND"		{ if(parse == 1) parse = 0; return PROTOEND; }
"//"[^\r\n]*             { /* skip comments */ }
[a-zA-Z"][a-zA-Z0-9_]* { if(parse == 1) { yylval.str = strdup(yytext); return STRING; }}
\n
. 		

%%
int yywrap()
{
	return 1;
}
