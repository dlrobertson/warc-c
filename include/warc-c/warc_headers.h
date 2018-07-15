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

#ifndef WARC_C_WARC_HEADERS
#define WARC_C_WARC_HEADERS

#include <warc-c/bytes_field.h>

#define WARC_HEADERS_INC 10

struct warc_header {
  char *name;
  struct bytes_field *value;
};

struct warc_header *warc_header_create(char *name, struct bytes_field *value);

void warc_header_free(struct warc_header *);

struct warc_headers {
  struct warc_header **store;
  size_t len;
  size_t cap;
};

int warc_headers_init(struct warc_headers *headers);

struct warc_header *warc_headers_add(struct warc_headers *headers, char *name,
                                     struct bytes_field *value);

struct warc_header *warc_headers_find(struct warc_headers *headers, const char *name);

struct bytes_field *warc_headers_get(struct warc_headers *headers, const char *name);

void warc_headers_free(struct warc_headers *headers);

#define FOREACH_HEADER(hdrs, tmp)                                                                  \
  for (tmp = (hdrs)->store; tmp < (hdrs)->store + (hdrs)->len; ++tmp)

#endif
