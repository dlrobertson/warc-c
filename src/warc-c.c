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

#include <stdlib.h>

#include <warc-c/warc-c.h>
#include <warc-c/warc_parser.h>

#include "warc.tab.h"
#define YYSTYPE WARCYYSTYPE
#include "warc.lex.h"

// FIXME(dlrobertson): This does not work at all.
struct warc_entry *warc_parse_buffer(const char *bytes, unsigned int len, int debug) {
  yyscan_t scanner;
  YY_BUFFER_STATE buf;

  warcyylex_init(&scanner);

  if (debug) {
    warcyydebug = 1;
    warcyyset_debug(1, scanner);
  }

  buf = warcyy_scan_bytes(bytes, len, scanner);

  warcyy_delete_buffer(buf, scanner);
  warcyylex_destroy(scanner);

  return NULL;
}

struct warc_entry *warc_parse_file(FILE *f, int debug) {
  struct warc_parser *parser = warc_parser_create(debug);

  if (!parser) {
    return NULL;
  }

  if (warc_parser_parse_file(parser, f)) {
    return NULL;
  } else {
    return warc_parser_consume(parser);
  }
}
