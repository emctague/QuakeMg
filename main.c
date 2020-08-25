//
//  main.c
//  
//
//  Created by Ethan McTague on 2020-08-25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qlist.h"
#include "qinfo.h"

int main(void) {
  struct qlist_entry *entries = NULL;
  int num_entries = 0;
  qlist_resolve("master.ioquake3.org:27950", &entries, &num_entries);

  for (int i = 0; i < num_entries; i++) {
    char result[23] = {0};
    snprintf(result, 23, "%u.%u.%u.%u:%u", entries[i].ip[0], entries[i].ip[1], entries[i].ip[2], entries[i].ip[3], entries[i].port);
    
    printf("%s\n", result);
    
    struct qinfo_result res = qinfo_query(result);
    
    for (int i = 0; i < res.num_properties; i++) {
      printf("%s = %s\n", res.properties[i].key, res.properties[i].value);
      free(res.properties[i].key);
      free(res.properties[i].value);
    }
    
    free(res.properties);
  }
  free(entries);
}
