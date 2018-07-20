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

#include <warc-c/warc_entry.h>
#include <warc-c/warc_file.h>
#include <warc-c/warc_headers.h>
#include <warc-c/warc_parser.h>

#include "warc.tab.h"
#define YYSTYPE WARCYYSTYPE
#include "warc.lex.h"

struct warc_parser {
  enum warc_parser_state state;
  int body_clrf_count;
  struct warc_entry *entry;
  struct warc_file *file;
  yyscan_t scanner;
};

struct warc_parser *warc_parser_create(int debug) {
  struct warc_parser *parser = (struct warc_parser *)malloc(sizeof(struct warc_parser));
  if (parser) {
    parser->entry = (struct warc_entry *)malloc(sizeof(struct warc_entry));
    if (parser->entry) {
      if (!warc_entry_init(parser->entry)) {
        parser->state = PARSER_STATE_SUCCESS;
        if (warcyylex_init_extra(parser, &parser->scanner)) {
          warc_parser_free(parser);
        } else {
          if (debug) {
            warcyydebug = 1;
            warcyyset_debug(1, parser->scanner);
          } else {
            warcyydebug = 0;
            warcyyset_debug(0, parser->scanner);
          }
          parser->file = warc_file_create();
          if (parser->file) {
            return parser;
          } else {
            warc_parser_free(parser);
          }
        }
      } else {
        free(parser->entry);
        free(parser);
      }
    } else {
      free(parser);
    }
  }
  return NULL;
}

void warc_parser_free(struct warc_parser *parser) {
  if (parser) {
    if (parser->entry) {
      warc_entry_free(parser->entry);
    }
    if (parser->file) {
      warc_file_free(parser->file);
    }
    if (parser->scanner) {
      warcyylex_destroy(parser->scanner);
    }
    free(parser);
  }
}

struct warc_entry* parser_consume_entry(struct warc_parser *parser) {
  struct warc_entry *entry = parser->entry;
  parser->entry = (struct warc_entry *)malloc(sizeof(struct warc_entry));
  if (parser->entry) {
    if (!warc_entry_init(parser->entry)) {
      return entry;
    }
  }
  warc_entry_free(entry);
  warcyyerror(parser->scanner, parser, "%s\n", "Error: No Memory!");
  return NULL;
}

void parser_push_entry(struct warc_parser *parser, struct warc_entry *entry) {
  if (entry) {
    warc_file_add(parser->file, entry);
  }
}

void parser_add_header(struct warc_parser *parser, char *name, struct bytes_field *value) {
  if (!warc_headers_add(&parser->entry->headers, name, value)) {
    parser->state = PARSER_STATE_NO_MEM;
    warcyyerror(parser->scanner, parser, "%s\n", "Error: No Memory!");
    bytes_field_free(value);
    free(name);
  }
}

void parser_set_block(struct warc_parser *parser, struct bytes_field *value) {
  parser->entry->block = value;
}

void parser_extend_block(struct warc_parser *parser, struct bytes_field *value) {
  if (!bytes_field_extend(parser->entry->block, value)) {
    warcyyerror(parser->scanner, parser, "%s\n", "Error: No Memory!");
  }
  bytes_field_free(value);
}

void parser_set_version(struct warc_parser *parser, int major, int minor) {
  parser->entry->version.major = major;
  parser->entry->version.minor = minor;
}

struct warc_file *warc_parser_consume(struct warc_parser *parser) {
  struct warc_file *file = NULL;
  if (parser->file) {
    file = parser->file;
    parser->file = NULL;
  }
  warc_parser_free(parser);
  return file;
}

enum warc_parser_state warc_parser_state(struct warc_parser *parser) { return parser->state; }

void warc_parser_set_state(struct warc_parser *parser, enum warc_parser_state state) {
  parser->state = state;
}

void *warc_parser_scanner(struct warc_parser *parser) { return &parser->scanner; }

enum warc_parser_state warc_parser_parse_file(struct warc_parser *parser, FILE *f) {
  int err;

  warcyyrestart(f, parser->scanner);

  err = warcyyparse(parser->scanner, parser);

  return (err && !parser->state) ? PARSER_STATE_MALFORMED : parser->state;
}

int flex_body_hit_crlf(struct warc_parser *parser) { return parser->body_clrf_count++; }

void flex_body_hit_noncrlf(struct warc_parser *parser) { parser->body_clrf_count = 0; }

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
