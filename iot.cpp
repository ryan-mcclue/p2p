// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(TEST_BUILD)
#define PROFILER
#endif

// https://www.youtube.com/watch?v=Iw5Y_-vsGac&list=PL0xRBLFXXsP4AqkUr9XHdFJwfwN-dy9oS 

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

GLOBAL BencodeNode nil_bencode_node =
{
  .type = NODE_TYPE_NULL,
  .integer_value = 0,
  .first = &nil_bencode_node,
  .next = &nil_bencode_node
};

INTERNAL BencodeNode *
bencode_nil_node(void)
{
  return &nil_bencode_node;
}

INTERNAL b32
is_bencode_nil_node(BencodeNode *node)
{
  return (node == NULL || node == &nil_bencode_node || node->type == NOTE_TYPE_NULL);
}

// WANT TO MINIMISE CODEPATHS!

//     MD_ParseResult result = MD_ZERO_STRUCT;
//     result.node = MD_NilNode();
//     return result;
// ZII:
//   - have all codepaths be able to operate on 0 input
//   - often code can work with 'empty' values instead of explicit validity checking
//   - nil nodes if wanting to read null; so not always required if say just iterating through a list 
//     return nil nodes and operate on in linked list traversal?
// reading a null pointer:
//   - return a nil struct so reading guaranteed

// literal invalid cases like failed allocation, i.e. operation can not possibly proceed
//   - fail early into callstack!

// 1. error information available in addition to, rather than instead of
//    in affect, 'side-channel' of information much more powerful
//    the message can contain information about what code set it and when, e.g. callstack, colour etc.
// 2. don't needlessly validite results
// error information in-addition to returned type, so usage code can inspect errors if it wants to
// error handling is code running smoothly in the presence of errors, with logs accumulated later if need be
// so, error (more like message) when want to know why it occurred
//   struct Result {
//     nodes answer;
//     error_list errors;
//   };
//   Result res = parse();
//   for (err in errors);
// if (errors->max_error_type == error_type_null);
// if (errors->max_error_type > error_type_null);

// fewer types means fewer codepaths

// IMPORTANT: code only works if run through a debugger and tested etc.
// however, as the number of all possible codepaths is huge, it's unfeasible to test all
// so, aim to reduce 'effective' codepaths
//
// want return value to have concrete guarantees, i.e. not a 'sum' type (so go against discriminated union)
// stateful code is where input and output same, i.e. pointer. locally stateful code is fine
// stateful code requires codepaths to make assumptions, which can often be wrong



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
    first = consume_bencode_list(arena, str);
    compound_node_type = NODE_TYPE_DICTIONARY;
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

// ecc (ellipitcal curve cryptography)
// linux fnmatch() for glob pattern checking?
// linux kernel crypto API:
//   * rng
//   * message digest same as hash (md5)
//     keyed message digest uses hash and key (HMAC)
//   * checksums aren't cryptographic
//   * cipher is umbrella term for encryption, e.g. symmetric(AES)/asymmetric(RSA)
INTERNAL void
explore(void)
{
#if 0
static int create_alg(const char *alg)
{
	struct sockaddr_alg salg;
	int sk;

	sk = socket(PF_ALG, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
	if (sk < 0)
		return -1;

	memset(&salg, 0, sizeof(salg));
	salg.salg_family = AF_ALG;
	strcpy((char *) salg.salg_type, "hash");
	strcpy((char *) salg.salg_name, alg);

	if (bind(sk, (struct sockaddr *) &salg, sizeof(salg)) < 0) {
		close(sk);
		return -1;
	}

	return sk;
}
#endif

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

  String8 str = str8_lit("l4:ryani-32ee");
  BencodeNode *node = consume_bencode_element(perm_arena, &str);

  // git show HEAD^:iot.cpp >> iot-old.cpp
  
  // sha1 more bits than md5, so more complex?
  // really sha1 fine over more secure sha256 as 1 in 9quintillion
  // meow hash probably better for large data

  // central tracker server stores peers

  // RPC is a broad concept. REST would be an implementation of RPC
  
  // nodes are distributed servers
  //  {src, dest, body}
  // clients interact with nodes via an RPC
  //  {type, id, in_reply_to, ...}

  // with networking in a 'moderated' environment, don't really have to go through effort of security
  // start simple with TCP and non-binary packing
  //
  // server: 1. inform all players of state when they connect:
  //              * assign authority to new player object 
  //         2. inform all players of updates to state
  //
  // handle_incoming_messages();
  // render_frame();
  // server_broadcast_updated_entities();
  // client_send_updated_authority_entities(); 

  explore();

  profiler_end_and_print();

  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
