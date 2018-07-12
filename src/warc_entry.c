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

#include <warc-c/warc_entry.h>

int warc_entry_init(struct warc_entry *entry) {
  struct warc_header** headers = malloc(sizeof(struct warc_header*) * WARC_HEADERS_INC);
  if (headers) {
    entry->version = NULL;
    entry->headers.len = 0;
    entry->headers.cap = WARC_HEADERS_INC;
    entry->headers.headers = headers;
    entry->block = NULL;
    return 0;
  } else {
    return -1;
  }
}

void warc_entry_free(struct warc_entry *entry) {
  struct warc_header** tmp;
  if (entry) {
    if (entry->version) {
      free(entry->version);
    }
    //FOREACH_HEADER(mod->headers, header) {
    for(tmp = (entry->headers).headers;
        tmp < (entry->headers).headers + (entry->headers).len;
        ++tmp) {
      warc_header_free(*tmp);
    }
    free(entry);
  }
}
