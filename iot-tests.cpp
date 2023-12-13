// SPDX-License-Identifier: zlib-acknowledgement
#include "iot.cpp"

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>

extern "C" {
#include <cmocka.h>
}

void
test_byte_string(void **state)
{
  String8 decoded = decode_bencode(str8_lit("4:ryan"));
  assert_int_equal(decoded.size, 4);
  assert_memory_equal((u8 *)decoded.content, "ryan", decoded.size);

  decoded = decode_bencode(str8_lit("2:ryan"));
  assert_int_equal(decoded.size, 2);
  assert_memory_equal((u8 *)decoded.content, "ry", decoded.size);
}

void
test_integer(void **state)
{
  String8 decoded = decode_bencode(str8_lit("i10e"));
  assert_int_equal(decoded.size, 2);
  assert_memory_equal((u8 *)decoded.content, "10", decoded.size);

  decoded = decode_bencode(str8_lit("i-122e"));
  assert_int_equal(decoded.size, 4);
  assert_memory_equal((u8 *)decoded.content, "-122", decoded.size);
}

int 
main(void)
{
	const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_byte_string),
    cmocka_unit_test(test_integer),
  };

  int cmocka_res = cmocka_run_group_tests(tests, NULL, NULL);

  return cmocka_res;
}
