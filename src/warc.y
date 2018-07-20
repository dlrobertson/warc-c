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

#include <warc-c/warc_parser.h>
#include "warc.tab.h"
#include "warc.lex.h"

%}

%define api.pure full
%define api.prefix {warcyy}
%define parse.trace
%define parse.error verbose
%lex-param {void* scanner}
%parse-param {void* scanner}{struct warc_parser* parser}

%union {
  char* str;
  struct bytes_field* bytes;
  struct warc_entry* record;
  int num;
}

/* Well known token strings */
%token CRLF
%token COLON
%token NUMBER
%token VERSION
%token BLOCK
%token TOKEN TEXT

%type<bytes> TEXT
%type<bytes> BLOCK

%type<str> TOKEN
%type<str> field_name
%type<str> VERSION

%type<num> NUMBER

%type<record> warc_record


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

warc_file:
  warc_record {
    parser_push_entry(parser, $1);
  } |
  warc_record warc_file {
    parser_push_entry(parser, $1);
  }
  ;

warc_record:
  header CRLF block CRLF {
    $$ = parser_consume_entry(parser);
  }
  ;

block:
  BLOCK CRLF {
    parser_set_block(parser, $1);
  } |
  block BLOCK CRLF {
    parser_extend_block(parser, $2);
  }
  ;

header:
  version warc_fields CRLF
  ;

version:
  VERSION '/' NUMBER '.' NUMBER CRLF {
    parser_set_version(parser, $3, $5);
  }
  ;

warc_fields:
  warc_fields CRLF named_field |
  named_field
  ;

named_field:
  field_name COLON {
    parser_add_header(parser, $1, NULL);
  } |
  field_name COLON TEXT {
    parser_add_header(parser, $1, $3);
  }
  ;

field_name:
  TOKEN { $$ = $1; }
  ;

%%
