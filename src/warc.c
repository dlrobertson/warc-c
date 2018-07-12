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

#include <warc.h>

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
  struct warc_entry* entry = malloc(sizeof(struct warc_entry));

  if (!entry || warcyylex_init(&scanner)) {
    return NULL;
  } else if (warc_entry_init(entry)) {
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

struct bytes_field* bytes_field_copy(const struct bytes_field* value) {
  struct bytes_field* hv = malloc(sizeof(struct bytes_field));
  if (hv && hv->len > 0) {
    hv->len = value->len;
    hv->bytes = malloc(hv->len);
    if (hv->bytes) {
      memcpy(hv->bytes, value->bytes, hv->len);
      return hv;
    } else {
      free(hv);
      return NULL;
    }
  } else {
    return NULL;
  }
}

struct warc_header* warc_header_create(const char* name, struct bytes_field* value) {
  struct warc_header* header = malloc(sizeof(struct warc_header));
  if (header && name) {
    header->name = strdup(name);
    if (header->name) {
      if (value) {
        header->value = value;
      } else {
        // Header has no value.
        header->value = NULL;
      }
      return header;
    } else {
      free(header);
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
      if (header->value->bytes) {
        free(header->value->bytes);
      }
      free(header->value);
    }
  }
}

static int warc_header_compar_sort(const void* hdr0, const void* hdr1) {
  return strcmp((*(const struct warc_header **)hdr0)->name,
                (*(const struct warc_header **)hdr1)->name);
}

static int warc_header_compar_find(const void* key, const void* hdr1) {
  return strcmp((const char*)key,
                (*(const struct warc_header **)hdr1)->name);
}

static int warc_headers_inner_add_header(struct warc_headers *headers,
                                         struct warc_header* header) {
  size_t new_len = headers->len + 1;
  struct warc_header** tmp;
  if (new_len >= headers->cap) {
    tmp = realloc(headers->headers, headers->cap + WARC_HEADERS_INC);
    if (tmp) {
      headers->cap += WARC_HEADERS_INC;
      headers->headers = tmp;
    } else {
      return -1;
    }
  }

  headers->headers[headers->len] = header;
  headers->len = new_len;
  return 0;
}

struct warc_header* warc_headers_add(struct warc_headers *headers, const char *name,
                                     struct bytes_field* value) {
  struct warc_header* header = NULL;
  if (headers) {
    header = warc_header_create(name, value);
    if (header) {
      if (!warc_headers_inner_add_header(headers, header)) {
        qsort(headers->headers, headers->len, sizeof(struct warc_header *),
              &warc_header_compar_sort);
        return header;
      } else {
        warc_header_free(header);
      }
    }
  }
  return NULL;
}

struct warc_header* warc_headers_find(struct warc_headers *headers, const char *name) {
    struct warc_header** elem = bsearch(name, headers->headers, headers->len,
                                        sizeof(struct warc_header *),
                                        &warc_header_compar_find);
    if (elem) {
        return *elem;
    } else {
        return NULL;
    }
}
