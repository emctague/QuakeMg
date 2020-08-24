#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "udp.h"
#include "qlist.h"

#define MAX_PACKET 4096

int main() {

  int num_entries;
  struct qlist_entry *entries = qlist_resolve("master.ioquake3.org:27950", &num_entries);

  for (int i = 0; i < num_entries; i++) {
    printf("%u.%u.%u.%u:%u\n", entries[i].ip[0], entries[i].ip[1], entries[i].ip[2], entries[i].ip[3], entries[i].port);
  }
  free(entries);
}


struct qlist_entry *qlist_resolve(const char *host, int *out_num_entries) {
  udp_t conn = resolve(host);
  
  udp_out(&conn, "\xFF\xFF\xFF\xFFgetservers 68", 0);

  char buf[MAX_PACKET];
  size_t bufSize = udp_in(&conn, buf, MAX_PACKET);
  udp_close(&conn);

  char *expect = "\xFF\xFF\xFF\xFFgetserversResponse\\";
  if (strncmp(buf, expect, strlen(expect))) {
    fprintf(stderr, "Error: Unexpected response to getservers request\n");
    return NULL;
  }


  struct qlist_entry *entries = NULL;
  int num_entries = 0;


  int base = strlen(expect);
  for (int i = base; i < bufSize; i += sizeof(struct qlist_entry)) {
    struct qlist_entry *entry = &buf[i];

    if (!strncmp(&entry->ip[0], "EOT", 3)) {
      break;
    }

    entries = realloc(entries, (num_entries + 1) * sizeof(struct qlist_entry));
    memcpy(&entries[num_entries], entry, sizeof(struct qlist_entry));

    entries[num_entries].port = __builtin_bswap16(entries[num_entries].port);
    num_entries++;
  }

  *out_num_entries = num_entries;

  return entries;
}
