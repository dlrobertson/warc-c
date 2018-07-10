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

#include <warc.h>
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
  const char* str;
  u_int8_t* bytes;
}

/* Well known token strings */
%token CRLF COLON END_OF_FILE

%token VERSION
%token BLOCK
%token TOKEN TEXT

%type<str> VERSION

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
  header CRLF BLOCK { printf("bison: warc_record!!!!!!!!!!\n"); }
  ;

header:
  version warc_fields CRLF { printf("bison: HEADER complete!\n"); }
  ;

version:
  VERSION CRLF { mod->version = strdup($1); }
  ;

warc_fields:
  warc_fields CRLF named_field { printf("bison: two warc fields\n"); } |
  named_field { printf("bison: named field!\n"); }
  ;

named_field:
  field_name COLON { printf("bison: field_name no text\n"); } |
  field_name COLON TEXT { printf("bison field_name with text\n"); }
  ;

field_name:
  TOKEN { printf("bison: field_name\n"); }
  ;

%%
