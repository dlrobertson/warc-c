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
#include <string.h>

#include <warc-c/bytes_field.h>
#include <warc-c/warc_headers.h>

struct warc_header *warc_header_create(char *name, struct bytes_field *value) {
  struct warc_header *header;
  if (name) {
    header = (struct warc_header *)malloc(sizeof(struct warc_header));
    if (header) {
      header->name = name;
      if (value) {
        header->value = value;
      } else {
        // Header has no value.
        header->value = NULL;
      }
      return header;
    }
  }
  return NULL;
}

void warc_header_free(struct warc_header *header) {
  if (header) {
    if (header->name) {
      free(header->name);
    }
    if (header->value) {
      bytes_field_free(header->value);
    }
    free(header);
  }
}

static int warc_header_compar_sort(const void *hdr0, const void *hdr1) {
  return strcmp((*(const struct warc_header **)hdr0)->name,
                (*(const struct warc_header **)hdr1)->name);
}

static int warc_header_compar_find(const void *key, const void *hdr1) {
  return strcmp((const char *)key, (*(const struct warc_header **)hdr1)->name);
}

static int warc_headers_inner_add_header(struct warc_headers *headers, struct warc_header *header) {
  size_t new_len = headers->len + 1;
  struct warc_header **tmp;
  if (new_len <= headers->cap) {
    tmp = (struct warc_header **)realloc(headers->store, (headers->cap + WARC_HEADERS_INC) *
                                                             sizeof(struct warc_header *));
    if (tmp) {
      headers->cap += WARC_HEADERS_INC;
      headers->store = tmp;
    } else {
      return -1;
    }
  }

  headers->store[headers->len] = header;
  headers->len = new_len;
  return 0;
}

int warc_headers_init(struct warc_headers *headers) {
  struct warc_header **store =
      (struct warc_header **)malloc(sizeof(struct warc_header *) * WARC_HEADERS_INC);
  if (store) {
    headers->len = 0;
    headers->cap = WARC_HEADERS_INC;
    headers->store = store;
    return 0;
  } else {
    return -1;
  }
}

struct warc_header *warc_headers_add(struct warc_headers *headers, char *name,
                                     struct bytes_field *value) {
  struct warc_header *header = NULL;
  if (headers) {
    header = warc_header_create(name, value);
    if (header) {
      if (!warc_headers_inner_add_header(headers, header)) {
        qsort(headers->store, headers->len, sizeof(struct warc_header *), &warc_header_compar_sort);
        return header;
      } else {
        warc_header_free(header);
      }
    }
  }
  return NULL;
}

struct warc_header *warc_headers_find(struct warc_headers *headers, const char *name) {
  struct warc_header **elem = (struct warc_header **)bsearch(
      name, headers->store, headers->len, sizeof(struct warc_header *), &warc_header_compar_find);
  return (elem) ? *elem : NULL;
}

struct bytes_field *warc_headers_get(struct warc_headers *headers, const char *name) {
  struct warc_header *elem = warc_headers_find(headers, name);
  return (elem) ? elem->value : NULL;
}

void warc_headers_free(struct warc_headers *headers) {
  struct warc_header **tmp;
  if (headers) {
    FOREACH_HEADER(headers, tmp) { warc_header_free(*tmp); }
    free(headers->store);
  }
}
