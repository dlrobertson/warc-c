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

#ifndef WARC_C_WARC_FILE
#define WARC_C_WARC_FILE

#include <warc-c/warc_entry.h>

struct warc_file_entry;

struct warc_file_entry *warc_file_entry_next(struct warc_file_entry *entry);

struct warc_file_entry *warc_file_entry_prev(struct warc_file_entry *entry);

struct warc_entry *warc_file_entry_item(struct warc_file_entry *entry);

struct warc_file;

struct warc_file *warc_file_create(void);

struct warc_file_entry *warc_file_head(struct warc_file *file);

struct warc_file_entry *warc_file_tail(struct warc_file *file);

struct warc_file_entry *warc_file_add(struct warc_file *file, struct warc_entry *entry);

void warc_file_free(struct warc_file *);

#define FOREACH_ENTRY(file, file_entry)                                                            \
  for (file_entry = warc_file_head(file); file_entry != NULL;                                      \
       file_entry = warc_file_entry_next(file_entry))

#define FOREACH_ENTRY_REVERSE(file, file_entry)                                                    \
  for (file_entry = warc_file_tail(file); file_entry != NULL;                                      \
       file_entry = warc_file_entry_prev(file_entry))

#endif
