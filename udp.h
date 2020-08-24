#pragma once
#include <netinet/in.h>


typedef struct {
  struct sockaddr_in sendAddr, recvAddr;
  socklen_t recvAddrLen;
  int socket;
} udp_t;

udp_t resolve(const char *fullAddr);
void udp_out(udp_t *udp, char *data, size_t len);
size_t udp_in(udp_t *udp, char *buf, size_t max);
void udp_close(udp_t *udp);

