#include "../lib/protocol.h"
#include "../lib/dgram.h"
#include "../lib/dsclient.h"
#include "../lib/dsserver.h"

static char usage[] =
     "usage: deskshare <command> [<args>]\n"
     "\tregister <username> <password> : register a new user\n"
     "\tlogin <username> <password> : refresh your host and ports\n"
     "\ttransmit <peername> : transmit video to a peer\n"
     "\treceive : wait for a call\n";

static void handle_register(char *username, char *password);
static void handle_login(char *username, char *password);
static void handle_transmit(char *peername);
static void handle_receive();

static int sfd, sfd_5000, sfd_5002;
static int serverport = 5009;
static char serverhost[] = "139.199.163.114";

int main(int argc, char *argv[])
{
     if (argc < 2) {
          printf("%s", usage);
          exit(EXIT_SUCCESS);
     }

     if (!strcmp(argv[1], "register"))
          handle_register(argv[2], argv[3]);
     else if (!strcmp(argv[1], "login"))
          handle_login(argv[2], argv[3]);
     else if (!strcmp(argv[1], "transmit"))
          handle_transmit(argv[2]);
     else if (!strcmp(argv[1], "receive"))
          handle_receive();
     else
          printf("%s", usage);

     exit(EXIT_SUCCESS);
}

static void handle_register(char *username, char *password)
{
     struct user_st user;
     struct addrinfo addr;
     char service[NI_MAXSERV];
     char *write_buf;
     char read_buf[BUFSIZ];
     size_t s;

     if ((sfd = make_dgram_server_socket(0)) == -1)
          exit(EXIT_FAILURE);

     strcpy(user.name, username);
     strcpy(user.password, password);
     s = put_register_info(&user, &write_buf);
     fprintf(stderr, "%s", write_buf);

     sprintf(service, "%d", serverport);
     if(make_addr(serverhost, service, &addr) == -1) {
          perror("make_addr:");
          exit(EXIT_FAILURE);
     }
     if (sendto(sfd, write_buf, s, 0, (struct sockaddr *) addr.ai_addr,
                addr.ai_addrlen) != s)
          perror("Error sending response");
     /* sfd = make_dgram_client_socket("127.0.0.1", 9999, 0); */
     /* if (write(sfd, write_buf, s) != s) { */
     /*      perror("Error sending response"); */
     /*      exit(EXIT_FAILURE); */
     /* } */
     free(write_buf);

     s = recvfrom(sfd, read_buf, BUFSIZ, 0, NULL, NULL);
     /* s = read(sfd, read_buf, BUFSIZ); */
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_register_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     printf("Register success!\n");
     close(sfd);
}

static void handle_login(char *username, char *password)
{
     struct user_st user;
     struct addrinfo addr;
     char service[NI_MAXSERV];
     char *write_buf;
     char read_buf[BUFSIZ];
     size_t s;

     if ((sfd_5000 = make_dgram_server_socket(5000)) == -1)
          exit(EXIT_FAILURE);
     if ((sfd_5002 = make_dgram_server_socket(5002)) == -1)
          exit (EXIT_FAILURE);

     strcpy(user.name, username);
     strcpy(user.password, password);
     s = put_login_info(&user, 5000, &write_buf);

     sprintf(service, "%d", serverport);
     if(make_addr(serverhost, service, &addr) == -1) {
          perror("make_addr:");
          exit(EXIT_FAILURE);
     }
     if (sendto(sfd_5000, write_buf, s, 0, (struct sockaddr *) addr.ai_addr,
                addr.ai_addrlen) != s)
          perror("Error sending response");

     /* if (write(sfd_5000, write_buf, s) != s) */
     /*      fprintf(stderr, "Error sending response\n"); */
     free(write_buf);

     s = recvfrom(sfd_5000, read_buf, BUFSIZ, 0, NULL, NULL);
     /* s = read(sfd_5000, read_buf, BUFSIZ); */
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_login_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     s = put_login_info(&user, 5002, &write_buf);
     if (sendto(sfd_5002, write_buf, s, 0, (struct sockaddr *) addr.ai_addr,
                addr.ai_addrlen) != s)
          perror("Error sending response");

     /* if (write(sfd_5002, write_buf, s) != s) */
     /*      fprintf(stderr, "Error sending response\n"); */
     free(write_buf);

     s = recvfrom(sfd_5002, read_buf, BUFSIZ, 0, NULL, NULL);
     /* s = read(sfd_5002, read_buf, BUFSIZ); */
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_login_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     printf("Login success!\n");
     close(sfd_5000);
     close(sfd_5002);
}

static void handle_transmit(char *peername)
{
     struct addrinfo addr;
     char service[NI_MAXSERV];
     char *write_buf;
     char read_buf[BUFSIZ];
     struct user_st peer;
     size_t s;

     if ((sfd = make_dgram_server_socket(0)) == -1)
          exit(EXIT_FAILURE);

     s = put_transmit_info(peername, &write_buf);

     sprintf(service, "%d", serverport);
     if(make_addr(serverhost, service, &addr) == -1) {
          perror("make_addr:");
          exit(EXIT_FAILURE);
     }
     if (sendto(sfd, write_buf, s, 0, (struct sockaddr *) addr.ai_addr,
                addr.ai_addrlen) != s)
          perror("Error sending response");
     /* if (write(sfd, write_buf, s) != s) */
     /*      fprintf(stderr, "Error sending response\n"); */
     free(write_buf);

     s = recvfrom(sfd, read_buf, BUFSIZ, 0, NULL, NULL);
     /* s = read(sfd, read_buf, BUFSIZ); */
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_transmit_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     get_transmit_ok_info(read_buf, &peer);
     printf("transmit video to %s@%s, audio to %s@%s.\n", peer.port_5000, peer.host,
            peer.port_5002, peer.host);
     transmitter_setup(peer.host, atoi(peer.port_5000), atoi(peer.port_5002));
     close(sfd);
}

static void handle_receive()
{
     receiver_setup();
}
