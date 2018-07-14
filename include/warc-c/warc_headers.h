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

#define WARC_HEADERS_INC 10

struct warc_headers {
  struct warc_header **headers;
  size_t len;
  size_t cap;
};

struct bytes_field *warc_headers_get(struct warc_headers *headers, const char *name);

struct warc_header *warc_headers_find(struct warc_headers *headers, const char *name);

struct warc_header *warc_headers_add(struct warc_headers *headers, const char *name,
                                     struct bytes_field *value);

#define FOREACH_HEADER(hdrs, tmp)                                                                  \
  for (tmp = (hdrs).headers; tmp < (hdrs).headers + (hdrs).len; ++tmp)

struct warc_header {
  char *name;
  struct bytes_field *value;
};

struct warc_header *warc_header_create(const char *name, struct bytes_field *value);

void warc_header_free(struct warc_header *);

#endif
