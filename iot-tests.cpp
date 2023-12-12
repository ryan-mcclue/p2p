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
test_string(void **state)
{
  String8 decoded = decode_bencode(str8_lit("4:ryan"));
  assert_int_equal(decoded.size, 4);
  assert_memory_equal((u8 *)decoded.content, "ryan", decoded.size);

  decoded = decode_bencode(str8_lit("2:ryan"));
  assert_int_equal(decoded.size, 2);
  assert_memory_equal((u8 *)decoded.content, "ry", decoded.size);
}

int 
main(void)
{
	const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_string),
  };

  int cmocka_res = cmocka_run_group_tests(tests, NULL, NULL);

  return cmocka_res;
}
