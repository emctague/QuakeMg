#pragma once
#include <stdint.h>

/* Represents an entry in a Quake 3 server list. */
struct __attribute__((packed)) qlist_entry {
  u_int8_t ip[4]; // IPv4: [0].[1].[2].[3]
  u_int16_t port; // Big endian, swapped during load.
  char pad; // backslash symbol, conventionally
};

/* Resolve a list of quake 3 servers given the master server hostname.
 * out_num_entries is populated with the number of entries. */
void qlist_resolve(const char *host, struct qlist_entry **out_entries, int *out_num_entries);

