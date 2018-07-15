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
#include <warc-c/warc_entry.h>
#include <warc-c/warc_headers.h>

struct warc_parser {
  enum warc_parser_state state;
  struct warc_entry *entry;
  void *scanner;
};

struct warc_parser *warc_parser_create(void *scanner) {
  struct warc_parser *prsr = (struct warc_parser *)malloc(sizeof(struct warc_parser));
  prsr->scanner = scanner;
  if (prsr) {
    prsr->entry = (struct warc_entry *)malloc(sizeof(struct warc_entry));
    if (prsr->entry) {
      if (!warc_entry_init(prsr->entry)) {
        prsr->state = PARSER_STATE_SUCCESS;
        return prsr;
      } else {
        free(prsr->entry);
        free(prsr);
      }
    } else {
      free(prsr);
    }
  }
  return NULL;
}

void warc_parser_free(struct warc_parser *prsr) {
  if (prsr) {
    if (prsr->entry) {
      free(prsr->entry);
    }
    free(prsr);
  }
}

void parser_add_header(struct warc_parser *prsr, const char *name, struct bytes_field *value) {
  if (!warc_headers_add(&prsr->entry->headers, name, value)) {
    prsr->state = PARSER_STATE_NO_MEM;
    warcyyerror(prsr->scanner, prsr, "%s\n", "Error: No Memory!");
  }
}

void parser_set_block(struct warc_parser *prsr, struct bytes_field *value) {
  prsr->entry->block = value;
}

void parser_set_version(struct warc_parser *prsr, char *version) { prsr->entry->version = version; }

struct warc_entry *warc_parser_consume(struct warc_parser *prsr) {
  struct warc_entry *ret = NULL;
  if (prsr->entry) {
    ret = prsr->entry;
    prsr->entry = NULL;
  }
  warc_parser_free(prsr);
  return ret;
}

enum warc_parser_state warc_parser_state(struct warc_parser *prsr) { return prsr->state; }

void warc_parser_set_state(struct warc_parser *prsr, enum warc_parser_state state) {
  prsr->state = state;
}

void *warc_parser_scanner(struct warc_parser *prsr) { return prsr->scanner; }
