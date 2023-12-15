// SPDX-License-Identifier: zlib-acknowledgement

// in C++, would use overloading
#define BASE_FIELD_TYPE_OF(t) (_Generic(*(t*)(NULL), \
  u8: FIELD_TYPE_U8, \
  u8*: FIELD_TYPE_U8, \
  char*: FIELD_TYPE_STR, \
  const char*: FIELD_TYPE_STR, \
))
