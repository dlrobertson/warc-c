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

#include <stdarg.h>
#include <stdlib.h>

#include <warc-c/warc-c.h>
#include <warc-c/warc_parser.h>

#include "warc.tab.h"
#define YYSTYPE WARCYYSTYPE
#include "warc.lex.h"

// FIXME(dlrobertson): This does not work at all.
struct warc_entry *warc_parse_buffer(const char *bytes, unsigned int len) {
  yyscan_t scanner;
  YY_BUFFER_STATE buf;

  warcyylex_init(&scanner);

  buf = warcyy_scan_bytes(bytes, len, scanner);

  warcyy_delete_buffer(buf, scanner);
  warcyylex_destroy(scanner);

  return NULL;
}

struct warc_entry *warc_parse_file(FILE *f) {
  yyscan_t scanner;
  struct warc_parser *parser = warc_parser_create(&scanner);

  if (!parser) {
    return NULL;
  } else if (warcyylex_init_extra(parser, &scanner)) {
    warc_parser_free(parser);
    return NULL;
  }

  // FIXME(dlroberton): remove this and add a parameter
  // to enable the equivalent of the following.
  //
  // warcyydebug = 1;
  // warcyyset_debug(1, scanner);
  warcyyrestart(f, scanner);
  if (!warcyyparse(scanner, parser) && !warc_parser_state(parser)) {
    warcyylex_destroy(scanner);
    return warc_parser_consume(parser);
  } else {
    warc_parser_free(parser);
    warcyylex_destroy(scanner);
    return NULL;
  }
}

int warcyyerror(void *scanner, struct warc_parser *parser, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  // If an error has already occurred don't overwrite it.
  if (!warc_parser_state(parser)) {
    fprintf(stderr, "%s\n", "Setting the parser state to an error!");
    warc_parser_set_state(parser, PARSER_STATE_MALFORMED);
  }
  return warc_parser_state(parser);
}
