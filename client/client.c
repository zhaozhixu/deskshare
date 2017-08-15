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
static char serverhost[] = "127.0.0.1";

int main(int argc, char *argv[])
{
     if (argc < 2) {
          printf("%s", usage);
          exit(EXIT_SUCCESS);
     }

     if ((sfd = make_dgram_client_socket(serverhost, serverport, 0)) == -1)
          return (EXIT_FAILURE);
     if ((sfd_5000 = make_dgram_client_socket(serverhost, serverport, 5000)) == -1)
          return (EXIT_FAILURE);
     if ((sfd_5002 = make_dgram_client_socket(serverhost, serverport, 5002)) == -1)
          return (EXIT_FAILURE);

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
     char *write_buf;
     char read_buf[BUFSIZ];
     size_t s;

     strcpy(user.name, username);
     strcpy(user.password, password);
     s = put_register_info(&user, &write_buf);
     fprintf(stderr, "%s", write_buf);

     /* sfd = make_dgram_client_socket("127.0.0.1", 9999, 0); */
     if (write(sfd, write_buf, s) != s) {
          perror("Error sending response");
          exit(EXIT_FAILURE);
     }
     free(write_buf);

     s = read(sfd, read_buf, BUFSIZ);
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_register_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     printf("Register success!\n");
}

static void handle_login(char *username, char *password)
{
     struct user_st user;
     char *write_buf;
     char read_buf[BUFSIZ];
     size_t s;

     strcpy(user.name, username);
     strcpy(user.password, password);
     s = put_login_info(&user, 5000, &write_buf);
     if (write(sfd_5000, write_buf, s) != s)
          fprintf(stderr, "Error sending response\n");
     free(write_buf);

     s = read(sfd_5000, read_buf, BUFSIZ);
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_login_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     s = put_login_info(&user, 5002, &write_buf);
     if (write(sfd_5002, write_buf, s) != s)
          fprintf(stderr, "Error sending response\n");
     free(write_buf);

     s = read(sfd_5002, read_buf, BUFSIZ);
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_login_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     printf("Login success!\n");
}

static void handle_transmit(char *peername)
{
     char *write_buf;
     char read_buf[BUFSIZ];
     struct user_st peer;
     size_t s;

     s = put_transmit_info(peername, &write_buf);
     if (write(sfd, write_buf, s) != s)
          fprintf(stderr, "Error sending response\n");
     free(write_buf);

     s = read(sfd, read_buf, BUFSIZ);
     if (s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
     }

     if (!is_transmit_ok(read_buf, s)) {
          printf("%s", read_buf);
          return;
     }

     get_transmit_ok_info(read_buf, &peer);
     transmitter_setup(peer.host, atoi(peer.port_5000), atoi(peer.port_5002));
}

static void handle_receive()
{
     pid_t pid;

     pid = fork();
     if (pid == 0)
          receiver_setup();
}
