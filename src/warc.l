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

%option noinput nounput noyywrap nodefault
%option bison-bridge reentrant
%option header-file="src/warc.lex.h"
%option extra-type="struct warc_parser *"
%option debug
%option prefix="warcyy"

%{
#include <stdarg.h>
#include <string.h>

#include <warc-c/warc_parser.h>
#include "warc.tab.h"
#define YYSTYPE WARCYYSTYPE
%}

TOKEN [\x21-\x39\x3b-\x7e]
TEXT [\x20-\x7e]
OCTET [\x00-\xff]
LWS \r\n*[\t ]+
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
  yylval->str = strndup(yytext, yyleng);
  return VERSION;
}

<<EOF>> {
  yyterminate();
}

<HEADER>{CRLF} {
  BEGIN(BODY);
  return CRLF;
}

<HEADER>{TOKEN}+ {
  yylval->str = strndup(yytext, yyleng);
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

<HEADER_VALUE>({TEXT}+|{LWS})+ {
  yylval->bytes = bytes_field_from_bytes((const uint8_t *)yytext, yyleng);
  if(yylval->bytes) {
    return TEXT;
  } else {
    warcyyerror(warc_parser_scanner(yyextra), yyextra, "flex: Unable to allocate memory\n");
  }
}

<BODY>{OCTET}*{CRLF}{CRLF} {
  yylval->bytes = bytes_field_from_bytes((const uint8_t *)yytext, yyleng);
  if(yylval->bytes) {
    BEGIN(INITIAL);
    return BLOCK;
  } else {
    warcyyerror(warc_parser_scanner(yyextra), yyextra, "flex: Unable to allocate memory\n");
  }
}

<BODY>.|\n {
  warcyyerror(warc_parser_scanner(yyextra), yyextra, "flex(BODY): Unknown token %s\n", yytext);
}

<HEADER>.|\n {
  warcyyerror(warc_parser_scanner(yyextra), yyextra, "flex(HEADER): Unknown token %s\n", yytext);
}

<HEADER_VALUE>.|\n {
  warcyyerror(warc_parser_scanner(yyextra), yyextra, "flex(HEADER_VALUE): Unknown token %s\n", yytext);
}

.|\n {
  warcyyerror(warc_parser_scanner(yyextra), yyextra, "flex(INITIAL): Unknown token %s\n", yytext);
}

%%

