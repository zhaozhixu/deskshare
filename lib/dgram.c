#include "dgram.h"

int make_dgram_server_socket(int portnum)
{
     struct addrinfo hints;
     struct addrinfo *result, *rp;
     int sfd, s;
     struct sockaddr_storage peer_addr;
     socklen_t peer_addr_len;
     char service[NI_MAXSERV];

     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_DGRAM;
     hints.ai_flags = AI_PASSIVE;
     hints.ai_protocol = 0;
     hints.ai_canonname = NULL;
     hints.ai_addr = NULL;
     hints.ai_next = NULL;

     sprintf(service, "%d", portnum);
     s = getaddrinfo(NULL, service, &hints, &result);
     if (s != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
          exit(EXIT_FAILURE);
     }

     for (rp = result; rp != NULL; rp = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype
                       rp->ai_protocol);
          if (sfd == -1)
               continue;

          if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
               break;

          close(sfd);
     }

     if (rp == NULL) {
          fprintf(stderr, "Could not bind\n");
          return -1;
     }

     freeaddrinfo(result);

     return sfd;
}

int make_dgram_client_socket(char *hostname, int portnum)
{
     struct addrinfo hints;
     struct addrinfo *result, *rp;
     int sfd, s;
     char service[NI_MAXSERV];

     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_DGRAM;
     hints.ai_flags = 0;
     hints.ai_protocol = 0;

     sprintf(service, "%d", portnum);
     s = getaddrinfo(hostname, service, &hints, &result);
     if (s != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
          return -1;
     }

     for (rp = result; rp != NULL; rp  = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);
          if (sfd = -1)
               continue;

          if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
               break;

          close(sfd);
     }

     freeaddrinfo(result);

     return sfd;
}
