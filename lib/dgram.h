#include "deskshare.h"

int make_dgram_server_socket(int portnum);
int make_dgram_client_socket(char *servhost, int servport, int localhost);
int make_addr(char *host, char *port, struct addrinfo *addr);
size_t send_and_recv(char *wr_buf, size_t len, char *rd_buf, int sfd, struct addrinfo *addr);
