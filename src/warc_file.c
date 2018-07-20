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

#include <assert.h>
#include <stdlib.h>

#include <warc-c/warc_entry.h>
#include <warc-c/warc_file.h>

struct warc_file_entry {
  struct warc_entry *entry;
  struct warc_file_entry *prev;
  struct warc_file_entry *next;
};

struct warc_file_entry *warc_file_entry_create_child(struct warc_file_entry *parent_entry,
                                                     struct warc_entry *entry) {
  struct warc_file_entry *child_entry = (struct warc_file_entry *)malloc(sizeof(struct warc_file_entry));

  assert(parent_entry != NULL);

  if (child_entry) {
    child_entry->prev = parent_entry;
    child_entry->next = parent_entry->next;
    child_entry->entry = entry;
    parent_entry->next = child_entry;
    if (child_entry->next) {
      child_entry->next->prev = child_entry;
    }
    return child_entry;
  }

  return NULL;
}

struct warc_file_entry *warc_file_entry_create_parent(struct warc_file_entry *child_entry,
                                                      struct warc_entry *entry) {
  struct warc_file_entry *parent_entry = (struct warc_file_entry *)malloc(sizeof(struct warc_file_entry));

  assert(child_entry != NULL);

  if (parent_entry) {
    parent_entry->next = child_entry;
    child_entry->prev = parent_entry;
    parent_entry->prev = child_entry->prev;
    if (parent_entry->prev) {
      parent_entry->prev->next = parent_entry;
    }
    return parent_entry;
  }
  return NULL;
}

struct warc_file_entry *warc_file_entry_next(struct warc_file_entry *entry) {
  return entry->next;
}

struct warc_file_entry *warc_file_entry_prev(struct warc_file_entry *entry) {
  return entry->prev;
}

struct warc_entry *warc_file_entry_item(struct warc_file_entry *entry) {
  return entry->entry;
}

struct warc_file {
  struct warc_file_entry *head;
  struct warc_file_entry *tail;
};

struct warc_file *warc_file_create(void) {
  struct warc_file *file = (struct warc_file *)malloc(sizeof(struct warc_file));
  if (file) {
    file->head = NULL;
    file->tail = NULL;
    return file;
  }
  return NULL;
}

struct warc_file_entry *warc_file_head(struct warc_file *file) {
  return file->head;
}

struct warc_file_entry *warc_file_tail(struct warc_file *file) {
  return file->tail;
}

struct warc_file_entry *warc_file_add(struct warc_file* file, struct warc_entry* entry) {
  if (file->tail) {
    file->tail = warc_file_entry_create_child(file->tail, entry);
    if (file->tail) {
      return file->tail;
    }
  } else {
    file->head = (struct warc_file_entry *)malloc(sizeof(struct warc_file_entry));
    if (file->head) {
      file->head->next = NULL;
      file->head->prev = NULL;
      file->head->entry = entry;
      file->tail = file->head;
      return file->head;
    }
  }
  return NULL;
}

void warc_file_free(struct warc_file* file) {
  struct warc_file_entry* entry;
  if (file) {
    FOREACH_ENTRY(file, entry) {
      warc_entry_free(entry->entry);
    }
    free(file);
  }
}
