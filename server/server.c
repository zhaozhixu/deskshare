#include "../lib/dgram.h"
#include "../lib/protocol.h"

static void process_req(char *buf, ssize_t n,
                   struct sockaddr_storage *peer_addr,
                   socklen_t peer_addr_len);
static void handle_register(char *buf, size_t n,
                            struct sockaddr_storage *dest_addr,
                            socklen_t addrlen);
static void handle_login(char *buf, size_t n,
                         char *host, char *service,
                         struct sockaddr_storage *dest_addr,
                         socklen_t addrlen);
static void handle_transmit(char *buf, size_t n,
                            struct sockaddr_storage *dest_addr,
                            socklen_t addrlen);

static int sfd;

int main(int argc, char *argv[])
{
     int portnum, s;
     struct sockaddr_storage peer_addr;
     socklen_t peer_addr_len;
     ssize_t nread;
     char buf[BUFSIZ];

     portnum = 5009;
     if ((sfd = make_dgram_server_socket(portnum)) == -1)
          return (EXIT_FAILURE);

     peer_addr_len = sizeof(struct sockaddr_storage);
     db_start("deskshare", "123");

     for(;;) {
          nread = recvfrom(sfd, buf, BUFSIZ, 0,
                           (struct sockaddr *)&peer_addr,
                           &peer_addr_len);
          if (nread == -1)
               continue;
          fprintf(stderr, "%s", buf);
          process_req(buf, nread, &peer_addr, peer_addr_len);
     }
}

static void process_req(char *buf, ssize_t n,
                   struct sockaddr_storage *peer_addr,
                   socklen_t peer_addr_len)
{
     ssize_t s;
     char host[NI_MAXHOST], service[NI_MAXSERV];

     s = getnameinfo((struct sockaddr *)peer_addr,
                     peer_addr_len, host, NI_MAXHOST,
                     service, NI_MAXSERV, NI_NUMERICSERV);
     if (s == 0)
          printf("Received %zd bytes from %s:%s\n",
                 n, host, service);
     else
          fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

     if (is_register(buf, n))
          handle_register(buf, n, peer_addr, peer_addr_len);
     else if (is_login(buf, n))
          handle_login(buf, n, host, service, peer_addr, peer_addr_len);
     else if (is_transmit(buf, n))
          handle_transmit(buf, n, peer_addr, peer_addr_len);
     else
          fprintf(stderr, "process_req: unrecognized request\n");
}

static void handle_register(char *buf, size_t n,
                            struct sockaddr_storage *dest_addr,
                            socklen_t addrlen)
{
     int res, user_id;
     size_t s;
     char *send_buf;
     char fail[] = "register fail";
     char fail_user[] = "already have this user";
     struct user_st user;
     struct user_st result;

     get_register_info(buf, &user);

     res = find_user(user.name, &result);
     if (res >= 1) {
          if (sendto(sfd, fail_user, sizeof(fail_user), 0,
                     (struct sockaddr *)dest_addr, addrlen)
              != sizeof(fail_user))
               fprintf(stderr, "Error sending response\n");
          return;
     }

     res = add_user(user.name, user.password, "", "", "", &user_id);
     printf("Result of adding a user was %d, user_id is %d\n",
            res, user_id);

     if (!res) {
          if (sendto(sfd, fail, sizeof(fail), 0,
                     (struct sockaddr *)dest_addr, addrlen)
              != sizeof(fail))
               fprintf(stderr, "Error sending response\n");
          return;
     }

     s = put_register_ok_info(&send_buf);
     if (sendto(sfd, send_buf, s, 0,
                (struct sockaddr *)dest_addr, addrlen) != s)
          fprintf(stderr, "Error sending response\n");
     free(send_buf);
}

static void handle_login(char *buf, size_t n,
                         char *host, char *service,
                         struct sockaddr_storage *dest_addr,
                         socklen_t addrlen)
{
     int res, port;
     size_t s;
     char fail_pw[] = "wrong password";
     char fail_un[] = "no such user";
     char fail[] = "login save fail";
     char *send_buf;
     struct user_st user;
     struct user_st result;

     get_login_info(buf, &user, &port);
     res = find_user(user.name, &result);

     if (res <= 0) {
          if (sendto(sfd, fail_un, sizeof(fail_un), 0,
                     (struct sockaddr *)&dest_addr, addrlen)
              != sizeof(fail_un))
               fprintf(stderr, "Error sending response\n");
          return;
     }

     if (strcmp(user.password, result.password)) {
          if (sendto(sfd, fail_pw, sizeof(fail_pw), 0,
                     (struct sockaddr *)dest_addr, addrlen)
              != sizeof(fail_pw))
               fprintf(stderr, "Error sending response\n");
          return;
     }

     if (port == 5000)
          res = update_user(result.id, result.name, result.password,
                            host, service, result.port_5002);
     else if (port == 5002)
          res = update_user(result.id, result.name, result.password,
                            host, result.port_5000, service);

     if (!res) {
          if (sendto(sfd, fail, sizeof(fail), 0,
                     (struct sockaddr *)dest_addr, addrlen)
              != sizeof(fail))
               fprintf(stderr, "Error sending response\n");
          return;
     }

     s = put_login_ok_info(port, &send_buf);
     if (sendto(sfd, send_buf, s, 0,
                (struct sockaddr *)dest_addr, addrlen) != s)
          fprintf(stderr, "Error sending response\n");
     free(send_buf);
}

static void handle_transmit(char *buf, size_t n,
                            struct sockaddr_storage *dest_addr,
                            socklen_t addrlen)
{
     int res;
     size_t s;
     char *peername;
     char *send_buf;
     struct user_st peer;
     char fail_user[] = "no such user";

     get_transmit_info(buf, &peername);
     res = find_user(peername, &peer);

     if (res <= 0) {
          if (sendto(sfd, fail_user, sizeof(fail_user), 0,
                     (struct sockaddr *)dest_addr, addrlen)
              != sizeof(fail_user))
               fprintf(stderr, "Error sending response\n");
          return;
     }

     s = put_transmit_ok_info(&peer, &send_buf);
     if (sendto(sfd, send_buf, s, 0, (struct sockaddr *)dest_addr,
                addrlen) != s)
          fprintf(stderr, "Error sending response\n");

     /* int cfd; */
     /* cfd = make_dgram_client_socket(peer.host, atoi(peer.port_5000), 0); */
     /* if (cfd == -1) */
     /*      return; */
     /* s = put_receive_info(peername, &send_buf); */
     /* if (write(cfd, send_buf, s) != s) */
     /*      fprintf(stderr, "Error sending response\n"); */
     /* close(cfd); */
     free(send_buf);
     free(peername);
}
