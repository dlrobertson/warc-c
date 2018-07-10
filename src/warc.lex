%option noyywrap nodefault
%option bison-bridge reentrant
%option header-file="src/warc.lex.h"
%{
#include <stdio.h>
#include <string.h>

#include <warc.h>
#include "warc.tab.h"
%}

TOKEN [\x21-\x39\x3b-\x7e]
TEXT [\x20-\x7e]
TEXTLWS [\x20-\x7e]
LWS \r\n*[\t ]*
CRLF \r\n
WS [\t ]*

%x HEADER
%x HEADER_VALUE
%x BODY

%%

{CRLF} {
  printf("CRLF\n");
  BEGIN(HEADER);
  return CRLF;
}

^WARC"/" {
  printf("WARC/\n");
  return VERSION;
}

[0-9]+ {
  printf("%d\n", atoi(yytext));
  return UINT;
}

"." {
  printf(".\n");
  return yytext[0];
}

<<EOF>> {
  return END_OF_FILE;
}

<HEADER>{CRLF} {
  BEGIN(BODY);
  return CRLF;
}

<HEADER>{TOKEN}+ {
  return TOKEN;
}

<HEADER>:{WS} {
  BEGIN(HEADER_VALUE);
  return COLON;
}

<HEADER_VALUE>{CRLF} {
  BEGIN(HEADER);
  return CRLF;
}

<HEADER_VALUE>{TEXT}+ {
  return TEXT;
}

<BODY>.*{CRLF}{CRLF} {
  BEGIN(INITIAL);
  return BLOCK;
}

<BODY>. {
  yyerror("flex: Unknown token: %s\n", yytext);
}

<HEADER_VALUE>. {
  yyerror("flex: Unknown token: %s\n", yytext);
}

.|\n {
  yyerror("flex: Unknown token: %s\n", yytext);
}

%%
