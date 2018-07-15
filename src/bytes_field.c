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

struct bytes_field *bytes_field_from_bytes(const uint8_t *bytes, size_t len) {
  struct bytes_field *field = NULL;
  if (len > 0) {
    field = (struct bytes_field *)malloc(sizeof(struct bytes_field));
    if (field) {
      field->len = len;
      field->bytes = (uint8_t *)malloc(field->len);
      if (field->bytes) {
        memcpy(field->bytes, bytes, field->len);
      } else {
        free(field);
        field = NULL;
      }
    }
  }
  return field;
}

struct bytes_field *bytes_field_copy(const struct bytes_field *value) {
  return bytes_field_from_bytes(value->bytes, value->len);
}

void bytes_field_free(struct bytes_field *field) {
  if (field) {
    if (field->bytes) {
      free(field->bytes);
    }
    free(field);
  }
}
