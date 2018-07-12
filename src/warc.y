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

%{
#include <stdint.h>
#include <string.h>

#include <warc-c/warc-c.h>
#include "warc.tab.h"
#define YYSTYPE WARCYYSTYPE
#include "warc.lex.h"

%}

%define api.pure full
%define parse.trace
%define parse.error verbose
%define api.prefix {warcyy}
%lex-param {void* scanner}
%parse-param {void* scanner}{struct warc_entry* mod}

%union {
  char* str;
  struct bytes_field* bytes;
}

/* Well known token strings */
%token CRLF COLON END_OF_FILE

%token VERSION
%token BLOCK
%token TOKEN TEXT

%type<bytes> TEXT
%type<str> TOKEN
%type<str> field_name
%type<str> VERSION
%type<bytes> BLOCK


/*
FIXME(dlrobertson): Add this to add WARC file parsing. This should probably
go in a separate parser?

warc_file:
  { printf("bison: EMPTY\n"); }|
  warc_record { printf("bison: warc_record\n"); } END_OF_FILE |
  warc_record warc_record END_OF_FILE
  ;
*/

%%

warc_record:
  header CRLF BLOCK { mod->block = $3; }
  ;

header:
  version warc_fields CRLF { }
  ;

version:
  VERSION CRLF {
    mod->version = $1;
  }
  ;

warc_fields:
  warc_fields CRLF named_field { } |
  named_field { }
  ;

named_field:
  field_name COLON {
    warc_headers_add(&mod->headers, $1, NULL);
  } |
  field_name COLON TEXT {
    warc_headers_add(&mod->headers, $1, $3);
  }
  ;

field_name:
  TOKEN { $$ = $1; }
  ;

%%
