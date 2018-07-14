/* Copyright (c) 2018 Daniel L. Robertson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

%option prefix="warcyy"
%option noinput nounput noyywrap nodefault
%option bison-bridge reentrant
%option header-file="src/warc.lex.h"

%{
#include <stdio.h>
#include <string.h>

#include <warc-c/warc-c.h>
#include "warc.tab.h"

#define YYSTYPE WARCYYSTYPE
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
  BEGIN(HEADER);
  return CRLF;
}

^WARC"/"1.0 {
  yylval->str = strdup(yytext);
  return VERSION;
}

<<EOF>> {
  return END_OF_FILE;
}

<HEADER>{CRLF} {
  BEGIN(BODY);
  return CRLF;
}

<HEADER>{TOKEN}+ {
  yylval->str = strdup(yytext);
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
  yylval->bytes = (struct bytes_field *)malloc(sizeof(struct bytes_field));
  yylval->bytes = bytes_field_from_bytes((const u_int8_t *)yytext, yyleng);
  if(yylval->bytes) {
    return TEXT;
  } else {
    warcyyerror("flex: Unable to allocate memory\n");
  }
}

<BODY>.*{CRLF}{CRLF} {
  yylval->bytes = (struct bytes_field *)malloc(sizeof(struct bytes_field));
  yylval->bytes = bytes_field_from_bytes((const u_int8_t *)yytext, yyleng);
  if(yylval->bytes) {
    BEGIN(INITIAL);
    return BLOCK;
  } else {
    warcyyerror("flex: Unable to allocate memory\n");
  }
}

<BODY>.|\n {
  warcyyerror("flex: Unknown token: %s\n", yytext);
}

<HEADER>.|\n {
  warcyyerror("flex: Unknown token: %s\n", yytext);
}

<HEADER_VALUE>.|\n {
  warcyyerror("flex: Unknown token: %s\n", yytext);
}

.|\n {
  warcyyerror("flex: Unknown token: %s\n", yytext);
}

%%
