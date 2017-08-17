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
          sfd = socket(rp->ai_family, rp->ai_socktype,
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

     printf("Bind successfully at %d.\n", portnum);
     freeaddrinfo(result);
     return sfd;
}

int make_dgram_client_socket(char *servhost, int servport, int localport)
{
     struct addrinfo hints;
     struct addrinfo *result, *rp;
     struct addrinfo *result_lo, *rp_lo;
     int sfd, s, s_lo;
     char service[NI_MAXSERV];
     char service_lo[NI_MAXSERV];

     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_DGRAM;
     hints.ai_flags = 0;
     hints.ai_protocol = 0;

     sprintf(service, "%d", servport);
     s = getaddrinfo(servhost, service, &hints, &result);
     if (s != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
          return -1;
     }

     sprintf(service_lo, "%d", localport);
     s_lo = getaddrinfo(NULL, service_lo, &hints, &result_lo);
     if (s_lo != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s_lo));
          return -1;
     }

     for (rp = result; rp != NULL; rp  = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);
          if (sfd == -1)
               continue;

          for (rp_lo = result_lo; rp_lo != NULL;
               rp_lo = rp_lo->ai_next) {
               if (bind(sfd, rp_lo->ai_addr, rp_lo->ai_addrlen) == 0)
                    break;
          }
          if (rp_lo == NULL)
               continue;

          if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
               break;
          perror("connect");

          close(sfd);
     }
     if (rp == NULL) {
          fprintf(stderr,
                  "make_dgram_client_socket: cannot make socket %d\n",
                  sfd);
          return -1;
     }

     freeaddrinfo(result);

     return sfd;
}

int make_addr(char *host, char *port, struct addrinfo *addr)
{
     int s, sfd;
     struct addrinfo hints;
     struct addrinfo *result, *rp;

     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_DGRAM;
     hints.ai_flags = AI_PASSIVE;
     hints.ai_protocol = 0;
     hints.ai_canonname = NULL;
     hints.ai_addr = NULL;
     hints.ai_next = NULL;

     s = getaddrinfo(host, port, &hints, &result);
     if (s != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
          return -1;
     }

     for (rp = result; rp != NULL; rp  = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);
          if (sfd == -1)
               continue;

          if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
               break;
     }
     close(sfd);
     if (rp == NULL)
          return -1;
     memmove(addr, rp, sizeof(struct addrinfo));
     /* addr = result; */

     return 0;
}
