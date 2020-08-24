#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

udp_t resolve(const char *fullAddr) {
  char *fullCopy = strdup(fullAddr);
  char *host = strtok(fullCopy, ":");
  char *port = strtok(NULL, "\0");
  struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_DGRAM, .ai_flags = AI_NUMERICSERV };
  struct addrinfo *addrs;
  udp_t output = {};

  if (getaddrinfo(host, port, &hints, &addrs) != 0) {
    perror("Cannot resolve address.");
    exit(1);
  }

  memcpy(&output.sendAddr, addrs->ai_addr, addrs->ai_addrlen);
  freeaddrinfo(addrs);
  free(fullCopy);

  output.recvAddr = output.sendAddr;
  output.recvAddr.sin_addr.s_addr = INADDR_ANY;
  output.recvAddrLen = sizeof(output.recvAddr);

  output.socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (output.socket < 0) { 
    perror("SOCK CREATION FUCKED");
    exit(2);
  }

  return output;
}


void udp_out(udp_t *udp, char *data, size_t len) {
  sendto(udp->socket, data, len ? len : strlen(data), 0, (const struct sockaddr*)&udp->sendAddr, sizeof(udp->sendAddr));
}

size_t udp_in(udp_t *udp, char *buf, size_t max) {
  size_t n = recvfrom(udp->socket, buf, max, 0, (struct sockaddr*)&udp->recvAddr, &udp->recvAddrLen);
  buf[n] = '\0';

  return n;
}

void udp_close(udp_t *udp) {
  close(udp->socket);
}
