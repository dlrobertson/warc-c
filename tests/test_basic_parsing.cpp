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
  struct warc_entry *entry = NULL;
  struct warc_header* header = NULL;
  FILE *f = fopen(TEST_FILES_EXAMPLES "/test.warc", "r");
  if (f) {
    entry = warc_parse_file(f);
    ASSERT_TRUE(entry != NULL);
    ASSERT_TRUE(entry->version != NULL);
    ASSERT_EQ(strcmp(entry->version, "WARC/1.0"), 0);

    // First Header
    header = warc_headers_find(&entry->headers, "Something");
    ASSERT_EQ(strcmp(header->name, "Something"), 0);
    ASSERT_EQ(strncmp((const char*)header->value->bytes,
                      "Else", header->value->len),
              0);

    // Second Header
    header = warc_headers_find(&entry->headers, "And0");
    ASSERT_EQ(strcmp(header->name, "And0"), 0);
    ASSERT_FALSE(header->value);

    // Third Header
    header = warc_headers_find(&entry->headers, "And1");
    ASSERT_EQ(strcmp(header->name, "And1"), 0);
    ASSERT_EQ(strncmp((const char*)header->value->bytes,
                      "Another", header->value->len),
              0);

    // Not a header
    header = warc_headers_find(&entry->headers, "Not Found");
    ASSERT_FALSE(header);

    // Check the block
    ASSERT_TRUE(entry->block);
    ASSERT_EQ(strncmp((const char*)entry->block->bytes,
                      "Hello, World!\r\n\r\n",
                      entry->block->len),
              0);
    warc_entry_free(entry);
  }
}
