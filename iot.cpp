// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(TEST_BUILD)
#define PROFILER
#endif

#include "base-inc.h"

typedef struct BencodeNode BencodeNode;
struct BencodeNode
{
  String8 value;
  BencodeNode *first;
  BencodeNode *next;
};

typedef enum
{
  TOKEN_TYPE_NULL = 0,
  TOKEN_TYPE_ERROR, 

} TOKEN_TYPE;

typedef struct Token Token;
struct Token
{
  TOKEN_TYPE type;
  RangeU64 range;
};

typedef struct TokenNode TokenNode;
struct TokenNode
{
  Token token;
  TokenNode *next;
};

typedef struct TokenList TokenList;
struct TokenList
{
  TokenNode *first, *last;
  u64 count;
};

INTERNAL TokenList
get_token_list(String8 str)
{
  TokenList result = ZERO_STRUCT;

  return result;
}

#if 0
INTERNAL Node *
parse_list_token()
{
  Node *first, *last;
  while (parsing)
  {
    advance_token(token);
    
    Node *child = parse_token(token);
    if (child != NULL) SLL_QUEUE_PUSH(first, last, child);

    if (token->type == ENDER) break;
  }
}

INTERNAL Node *
parse_token(token)
{
  if (token->type == LIST_START)
  {
    first_child = parse_list_token();
  }
  else if (token->type == DICTIONARY)
  {
    first_child = parse_dictionary_token();
  }
  else if (token->type == INTEGER)
  {
    node->integer_val = ;
  }
  else if (token->type == STRING)
  {

  }
  else
  {

  }

}
#endif

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
