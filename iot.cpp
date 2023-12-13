// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(TEST_BUILD)
#define PROFILER
#endif

#include "base-inc.h"

typedef enum
{
  NODE_TYPE_NULL,
  NODE_TYPE_INTEGER,
  NODE_TYPE_STRING,
  NODE_TYPE_LIST,
  NODE_TYPE_DICTIONARY,
} NODE_TYPE;

typedef struct BencodeNode BencodeNode;
struct BencodeNode
{
  NODE_TYPE type;
  union {
    String8 string_value;
    s32 integer_value;
  };
  BencodeNode *first;
  BencodeNode *next;
};

INTERNAL BencodeNode *
consume_bencode_integer(MemArena *arena, String8 *str)
{
  BencodeNode *node = NULL;

  u32 at = 1;
  if (at < str->size)
  {
    u32 token_start = at;
    while (at < str->size && str->content[at] != 'e')
    {
      at++;
    }
    
    node = MEM_ARENA_PUSH_STRUCT_ZERO(arena, BencodeNode);
    node->type = NODE_TYPE_INTEGER; 

    String8 integer_str = str8_substring(*str, 1, at);
    node->integer_value = str8_to_int(integer_str);
    at++;

    str->content += at;
    str->size -= at;
  }

  return node;
}

INTERNAL BencodeNode *
consume_bencode_string(MemArena *arena, String8 *str)
{
  BencodeNode *node = NULL;

  u32 at = 0;
  while (at < str->size && str->content[at] != ':')
  {
    at++;
  }

  String8 string_length_str = str8_prefix(*str, at);
  u32 string_length = str8_to_u64(string_length_str, 10);
  
  at++;

  if (at < str->size)
  {
    u32 node_start = at;
    u32 string_ch_count = 0;
    while (at < str->size && string_ch_count < string_length)
    {
      string_ch_count++;
      at++;
    }

    node = MEM_ARENA_PUSH_STRUCT_ZERO(arena, BencodeNode);
    node->type = NODE_TYPE_STRING; 

    node->string_value = str8_substring(*str, node_start, at);
    //at++;

    str->content += at;
    str->size -= at;
  }

  return node;
}

INTERNAL BencodeNode *
consume_bencode_element(MemArena *arena, String8 *str);
INTERNAL BencodeNode *
consume_bencode_list(MemArena *arena, String8 *str)
{
  BencodeNode *first_child = NULL, *last_child = NULL;

  str->content += 1;
  str->size -= 1;

  u32 at = 0;
  if (at < str->size)
  {
    while (str->content[at] != 'e')
    {
      BencodeNode *child = consume_bencode_element(arena, str);
      SLL_QUEUE_PUSH(first_child, last_child, child);
    }
  }

  return first_child;
}

INTERNAL BencodeNode *
consume_bencode_element(MemArena *arena, String8 *str)
{
  BencodeNode *result = NULL;
  BencodeNode *first = NULL;

  char ch = str->content[0];

  NODE_TYPE compound_node_type = NODE_TYPE_NULL;

  if (ch == 'i')
  {
    result = consume_bencode_integer(arena, str);
  }
  else if (isdigit(ch))
  {
    result = consume_bencode_string(arena, str);
  }
  else if (ch == 'l')
  {
    first = consume_bencode_list(arena, str);
    compound_node_type = NODE_TYPE_LIST;
  }
  else if (ch == 'd')
  {
  }
  else
  {
    UNREACHABLE();
  }

  if (result == NULL)
  {
    result = MEM_ARENA_PUSH_STRUCT_ZERO(arena, BencodeNode);
    result->type = compound_node_type; 
    result->first = first;
  }

  return result;
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

  String8 str = str8_lit("l4:ryan3:loce");
  BencodeNode *node = consume_bencode_element(perm_arena, &str);

  profiler_end_and_print();

  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
