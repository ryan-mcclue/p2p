// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(TEST_BUILD)
#define PROFILER
#endif

#include "base-inc.h"

INTERNAL String8
decode_bencode(String8 bencode_str)
{
  String8 res = ZERO_STRUCT;

  // l[elements]e
  if (bencode_str.content[0] == 'i')
  {
    String8 number = str8_chop_by_delim(bencode_str, str8_lit("e"));
    number = str8_advance(number, 1); 
    return number;
    //s64 num = str8_to_int(number);
  }
  else
  {
    String8 len = str8_chop_by_delim(bencode_str, str8_lit(":"));
    u32 l = str8_to_u64(len, 10);
    String8 str = str8_advance(bencode_str, len.size + 1);
    str.size = l;
    return str;
  }

  return res;
}

#if defined(TEST_BUILD)
int testable_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  global_debugger_present = linux_was_launched_by_gdb();
  MemArena *perm_arena = mem_arena_allocate_default();

  ThreadContext tctx = thread_context_allocate();
  tctx.is_main_thread = true;
  thread_context_set(&tctx);

#if defined(RELEASE_BUILD)
  linux_set_cwd_to_self();
#else
  linux_append_ldlibrary(str8_lit("./build"));
#endif

  profiler_init();

  String8List args_list = ZERO_STRUCT;
  for (s32 i = 1; i < argc; i += 1)
  {
    str8_list_push(perm_arena, &args_list, str8_cstr(argv[i]));
  }

  String8 decoded = decode_bencode(str8_lit("i-12222e"));
  PRINT_STR8(decoded);

  profiler_end_and_print();

  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
