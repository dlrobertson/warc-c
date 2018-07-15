// Copyright (c) 2018 Daniel L. Robertson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef WARC_C_WARC_PARSER
#define WARC_C_WARC_PARSER

#include <stdio.h>

#include <warc-c/bytes_field.h>
#include <warc-c/warc_entry.h>

struct warc_parser;

enum warc_parser_state {
  PARSER_STATE_SUCCESS = 0,
  PARSER_STATE_NO_MEM,
  PARSER_STATE_MALFORMED,
  PARSER_STATE_MAX
};

struct warc_parser *warc_parser_create(void *scanner);

void warc_parser_free(struct warc_parser *prsr);

void parser_add_header(struct warc_parser *prsr, const char *name, struct bytes_field *value);

void parser_set_block(struct warc_parser *prsr, struct bytes_field *value);

void parser_set_version(struct warc_parser *prsr, char *version);

struct warc_entry *warc_parser_consume(struct warc_parser *prsr);

enum warc_parser_state warc_parser_state(struct warc_parser *prsr);

void warc_parser_set_state(struct warc_parser *prsr, enum warc_parser_state state);

void *warc_parser_scanner(struct warc_parser *prsr);

#endif
