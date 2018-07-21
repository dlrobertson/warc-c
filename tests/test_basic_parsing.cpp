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

#include <cstdio>
#include <gtest/gtest.h>

extern "C" {
#include <warc-c/warc-c.h>
}

#include "util.hpp"

TEST(SimpleWarcFile, first) {
  struct warc_file *file = NULL;
  struct warc_file_entry *node = NULL;
  struct warc_entry *entry = NULL;
  struct warc_header *header = NULL;
  FILE *f = fopen(TEST_FILES_EXAMPLES "/test.warc", "r");
  int i = 0;
  file = warc_parse_file(f, 1);
  FOREACH_ENTRY(file, node) {
    ++i;
    if (i == 0) {
      ASSERT_TRUE(node);
      entry = warc_file_entry_item(node);
      ASSERT_TRUE(entry != NULL);
      ASSERT_EQ(entry->version.major, 1);
      ASSERT_EQ(entry->version.minor, 0);

      // First Header
      header = warc_headers_find(&entry->headers, "Something");
      ASSERT_TRUE(header);
      ASSERT_EQ(strcmp(header->name, "Something"), 0);
      ASSERT_EQ(strncmp((const char *)header->value->bytes, "Else", header->value->len), 0);

      // Second Header
      header = warc_headers_find(&entry->headers, "And0");
      ASSERT_TRUE(header);
      ASSERT_EQ(strcmp(header->name, "And0"), 0);
      ASSERT_FALSE(header->value);

      // Third Header
      header = warc_headers_find(&entry->headers, "And1");
      ASSERT_TRUE(header);
      ASSERT_EQ(strcmp(header->name, "And1"), 0);
      ASSERT_EQ(
          strncmp((const char *)header->value->bytes, "Multiple\r\n Lines", header->value->len), 0);

      // Not a header
      header = warc_headers_find(&entry->headers, "Not Found");
      ASSERT_FALSE(header);

      // Check the block
      ASSERT_TRUE(entry->block);
      ASSERT_EQ(strncmp((const char *)entry->block->bytes, "Hello,\r\nWorld!", entry->block->len),
                0);
    } else if (i == 1) {
      ASSERT_TRUE(node);
      entry = warc_file_entry_item(node);
      ASSERT_TRUE(entry != NULL);
      ASSERT_EQ(entry->version.major, 1);
      ASSERT_EQ(entry->version.minor, 0);

      // First Header
      header = warc_headers_find(&entry->headers, "Wait");
      ASSERT_TRUE(header);
      ASSERT_EQ(strcmp(header->name, "Wait"), 0);
      ASSERT_EQ(strncmp((const char *)header->value->bytes, "There", header->value->len), 0);

      // Check the block
      ASSERT_TRUE(entry->block);
      ASSERT_EQ(strncmp((const char *)entry->block->bytes, "Is another entry!", entry->block->len),
                0);
    }
  }
  ASSERT_EQ(i, 2);
  warc_file_free(file);
}

TEST(SimpleWarcFile, bbc) {
  struct warc_entry *entry = NULL;
  struct warc_file *file = NULL;
  struct warc_file_entry *node = NULL;
  struct warc_header *header = NULL;
  FILE *f = fopen(TEST_FILES_EXAMPLES "/bbc.warc", "r");
  int i = 0;
  file = warc_parse_file(f, 0);
  FOREACH_ENTRY(file, node) {
    ++i;
    entry = warc_file_entry_item(node);
    ASSERT_TRUE(entry != NULL);
    ASSERT_EQ(entry->version.major, 1);
    ASSERT_EQ(entry->version.minor, 0);

    // First Header
    header = warc_headers_find(&entry->headers, "WARC-Type");
    ASSERT_TRUE(header);
    ASSERT_EQ(strcmp(header->name, "WARC-Type"), 0);
    ASSERT_EQ(strncmp((const char *)header->value->bytes, "response", header->value->len), 0);
  }
  ASSERT_EQ(i, 1);
  warc_file_free(file);
}
