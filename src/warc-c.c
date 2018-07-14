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
  struct warc_entry *entry = (struct warc_entry *)malloc(sizeof(struct warc_entry));

  if (!entry) {
    return NULL;
  } else if (warc_entry_init(entry) || warcyylex_init(&scanner)) {
    warc_entry_free(entry);
    return NULL;
  }

  // FIXME(dlroberton): remove this and add a parameter
  // to enable it.
  warcyyset_debug(1, scanner);

  warcyyrestart(f, scanner);
  if (warcyyparse(scanner, entry)) {
    warcyylex_destroy(scanner);
    return entry;
  } else {
    warc_entry_free(entry);
    warcyylex_destroy(scanner);
    return NULL;
  }
}

void warcyyerror(const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}
