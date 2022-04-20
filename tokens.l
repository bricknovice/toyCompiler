/***Definition Section***/
%{
#include <string>
#include "node.h"
#include "parser.hpp"
// yylval = semantic value of the token
#define SAVE_TOKEN() (yylval.string = new std::string(yytext, yyleng))
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { return 1;}
%}

/***Rules Section***/
%% 
[ \t\n]			        ;
"return"                SAVE_TOKEN(); return TOKEN(TRETURN);
"if"                    SAVE_TOKEN(); return TOKEN(TIF);
"else"                  SAVE_TOKEN(); return TOKEN(TEL);
[a-zA-Z_][a-zA-Z0-9_]* 	SAVE_TOKEN(); return TIDENTIFIER;
[0-9]+\.[0-9]*			SAVE_TOKEN(); return TDOUBLE;
[0-9]+				    SAVE_TOKEN(); return TINTEGER;
"="				            return TOKEN(TEQUAL);
"=="                    	return TOKEN(TCEQ);
"!="                    	return TOKEN(TCNE);
"<"                     	return TOKEN(TCLT);
"<="                    	return TOKEN(TCLE);
">"                     	return TOKEN(TCGT);
">="                    	return TOKEN(TCGE);
"("                     	return TOKEN(TLPAREN);
")"                     	return TOKEN(TRPAREN);
"{"                     	return TOKEN(TLBRACE);
"}"                     	return TOKEN(TRBRACE);
"."                     	return TOKEN(TDOT);
","                    	    return TOKEN(TCOMMA);
"+"                     	return TOKEN(TPLUS);
"-"                     	return TOKEN(TMINUS);
"*"                     	return TOKEN(TMUL);
"/"                     	return TOKEN(TDIV);
"&"                         return TOKEN(TAND);
"|"                         return TOKEN(TOR);
"^"                         return TOKEN(TXOR);
"<<"                        return TOKEN(TLSFT);
">>"                        return TOKEN(TRSFT);
.                       	printf("Unknown token!n"); yyterminate();
%%
