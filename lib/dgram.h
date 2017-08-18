#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int make_dgram_server_socket(int portnum);
int make_dgram_client_socket(char *servhost, int servport, int localhost);
int make_addr(char *host, char *port, struct addrinfo *addr);
