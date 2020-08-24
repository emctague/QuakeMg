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

  struct qlist_entry *entries = NULL;
  int num_entries = 0;
  qlist_resolve("master.ioquake3.org:27950", &entries, &num_entries);

  for (int i = 0; i < num_entries; i++) {
    printf("%u.%u.%u.%u:%u\n", entries[i].ip[0], entries[i].ip[1], entries[i].ip[2], entries[i].ip[3], entries[i].port);
  }
  free(entries);
}


void qlist_resolve(const char *host, struct qlist_entry **out_entries, int *out_num_entries) {
  udp_t conn = resolve(host);
  
  udp_out(&conn, "\xFF\xFF\xFF\xFFgetservers 68", 0);

  char buf[MAX_PACKET];
  int done = 0;

  /* Iterate as multiple response packets may be sent. */
  while (!done) {

    /* Read response packet */
    size_t bufSize = udp_in(&conn, buf, MAX_PACKET);

    /* Validate response introduction string */
    char *expect = "\xFF\xFF\xFF\xFFgetserversResponse\\";
    if (strncmp(buf, expect, strlen(expect))) {
      fprintf(stderr, "Error: Unexpected response to getservers request\n");
      udp_close(&conn);
      return;
    }

    /* Iterate IPs in response */
    int base = strlen(expect);
    for (size_t i = base; i < bufSize; i += sizeof(struct qlist_entry)) {
      struct qlist_entry *entry = (struct qlist_entry*)&buf[i];

      /* Response end-case is an IP which looks like "EOT\0" when handled as chars */
      if (!strncmp((char*)&entry->ip[0], "EOT", 3)) {
        done = 1;
        break;
      }

      /* Append to entries, including swapping endianness of 16-bit port */
      *out_entries = realloc(*out_entries, (*out_num_entries + 1) * sizeof(struct qlist_entry));
      memcpy(&(*out_entries)[*out_num_entries], entry, sizeof(struct qlist_entry));
      (*out_entries)[*out_num_entries].port = __builtin_bswap16((*out_entries)[*out_num_entries].port);
      (*out_num_entries)++;
    }
  }

  udp_close(&conn);
}
