#include "qinfo.h"
#include "udp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PACKET 4096

struct qinfo_result qinfo_query(const char *host) {
  udp_t conn = resolve(host);
  udp_out(&conn, "\xFF\xFF\xFF\xFFgetinfo 0000", 0);

  char buf[MAX_PACKET];
  
  struct qinfo_result out = { NULL, 0 };

  /* Read response packet */
  size_t bufSize = udp_in(&conn, buf, MAX_PACKET);

  /* Validate response introduction string */
  char *expect = "\xFF\xFF\xFF\xFFinfoResponse\x0A\\";
  if (strncmp(buf, expect, strlen(expect))) {
    fprintf(stderr, "Error: Unexpected response to getservers request\n");
    udp_close(&conn);
    return (struct qinfo_result){NULL, 0};
  }

  char *result = &buf[strlen(expect)];
    
  char *word, *brkt;
  for (word = strtok_r(result, "\\", &brkt); word; word = strtok_r(NULL, "\\", &brkt)) {
    char *key = strdup(word);
    char *value = strdup(strtok_r(NULL, "\\", &brkt));
    
    out.properties = realloc(out.properties, (out.num_properties + 1) * sizeof(struct qinfo_property));
    out.properties[out.num_properties] = (struct qinfo_property){ key, value };
    out.num_properties++;
  }
  
  return out;
}
