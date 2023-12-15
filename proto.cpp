// SPDX-License-Identifier: zlib-acknowledgement

// TODO: for every frame just iterate over packets recieved and handle them iteratively?

// nodes are distributed servers
// clients interact with nodes via an RPC
struct Message
{
  String8 src;
  String8 dst;
  Body body/payload;
};

struct Body
{
  u32 type;
  u32 id; // this is like a per-node unique sequence number for logging?
  u32 in_reply_to_id;
  union
  {
    struct Echo {
      String8 echo;
    };
    struct Broadcast {

    };
    struct BroadcastOk {

    };
    struct Read {

    };
    struct ReadOk {

    };
    struct Topology
    {

    };
    ...
  };
};

Message handle_echo_message(Message msg)
{
  // may require a lock if interleaving, i.e. respond to other messages whilst recieving
  Message res = {
    .src = msg.dst,
    .dst = msg.src,
    .body = ;
  };

  return res;
}

Message handle_init_message()
{

}

// Paxos algorithm (a type of consensus algorithm) to ensure global uniqueness; otherwise:
// uuid is spec'd out as 128bit, typically incorporate various sources of randomness, e.g. system info and time based (so can roughly sort based on allocation time)
// ulid is another form
// could even form via a say f"{node.id}{self.id}" 

// TODO: network stats in a central server: log all messages sent, latency of messages etc.
