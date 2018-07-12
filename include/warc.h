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

#ifndef WARC_C_WARC
#define WARC_C_WARC

#include <stdio.h>

#define WARC_HEADERS_INC 10

void warcyyerror(const char *s, ...);

struct warc_entry *warc_parse_file(FILE *f);

struct warc_entry *warc_parse_buffer(const char *bytes, unsigned int len);

struct bytes_field {
  size_t len;
  u_int8_t* bytes;
};

struct warc_header {
  char* name;
  struct bytes_field* value;
};

struct warc_headers {
  struct warc_header** headers;
  size_t len;
  size_t cap;
};

struct warc_entry {
  char *version;
  struct warc_headers headers;
  struct bytes_field *block;
};

int warc_entry_init(struct warc_entry *entry);

void warc_entry_free(struct warc_entry *mod);

struct warc_header* warc_header_create(const char* name, struct bytes_field* value);

void warc_header_free(struct warc_header *);

struct bytes_field* bytes_field_copy(const struct bytes_field *value);

struct bytes_field* warc_headers_get(struct warc_headers *headers, const char *name);

struct warc_header* warc_headers_find(struct warc_headers *headers, const char *name);

struct warc_header* warc_headers_add(struct warc_headers *headers, const char *name,
                                     struct bytes_field* value);
#define FOREACH_HEADER(headers, tmp) \
    for(tmp = (headers).headers; tmp < (headers).headers + (headers).len; ++tmp)

#endif
