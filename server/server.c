#include "../lib/dgram.h"
#include "../lib/protocol.h"
#include "../lib/db.h"

static void process_req(char *buf, ssize_t n,
                   struct sockaddr_storage clt_addr,
                   socklen_t clt_addr_len);
static void handle_register(char *buf, size_t n,
                            struct sockaddr_storage *clt_addr,
                            socklen_t clt_addr_len);
static void handle_login(char *buf, size_t n,
                         char *host, char *service,
                         struct sockaddr_storage *clt_addr,
                         socklen_t clt_addr_len);
static void handle_transmit(char *buf, size_t n,
                            struct sockaddr_storage *clt_addr,
                            socklen_t clt_addr_len);

static int sfd;
static int serverport = DEFAULT_SERVER_PORT;

int main(int argc, char *argv[])
{
     ssize_t nread;
     char buf[BUFSIZ];
     struct sockaddr_storage clt_addr;
     socklen_t clt_addr_len;

     if ((sfd = make_dgram_server_socket(serverport)) == -1)
          return (EXIT_FAILURE);

     clt_addr_len = sizeof(struct sockaddr_storage);
     db_start("deskshare", "123");

     for(;;) {
          nread = recvfrom(sfd, buf, BUFSIZ, 0,
                           (struct sockaddr *)&clt_addr,
                           &clt_addr_len);
          if (nread == -1)
               continue;
          fprintf(stderr, "%s", buf);
          process_req(buf, nread, clt_addr, clt_addr_len);
     }
}

static void process_req(char *buf, ssize_t n,
                   struct sockaddr_storage clt_addr,
                   socklen_t clt_addr_len)
{
     ssize_t s;
     char host[NI_MAXHOST], service[NI_MAXSERV];

     s = getnameinfo((struct sockaddr *)&clt_addr,
                     clt_addr_len, host, NI_MAXHOST,
                     service, NI_MAXSERV, NI_NUMERICSERV);
     if (s == 0)
          printf("Received %zd bytes from %s:%s\n",
                 n, host, service);
     else
          fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

     if (is_register(buf, n))
          handle_register(buf, n, &clt_addr, clt_addr_len);
     else if (is_login(buf, n))
          handle_login(buf, n, host, service, &clt_addr, clt_addr_len);
     else if (is_transmit(buf, n))
          handle_transmit(buf, n, &clt_addr, clt_addr_len);
     else
          fprintf(stderr, "process_req: unrecognized request\n");
}

static int send_buf(char *buf, size_t len, struct sockaddr_storage *clt_addr, socklen_t clt_addr_len)
{
     if (sendto(sfd, buf, len, 0, (struct sockaddr *)clt_addr, clt_addr_len) != len) {
          fprintf(stderr, "Error sending response\n");
          return -1;
     }
     return 0;
}

static void handle_register(char *buf, size_t n,
                            struct sockaddr_storage *clt_addr,
                            socklen_t clt_addr_len)
{
     int res, user_id;
     size_t s;
     char *wr_buf;
     char fail[] = "register fail";
     char fail_user[] = "already have this user";
     struct user_st user;
     struct user_st result;

     get_register_info(buf, &user);

     res = find_user(user.name, &result);
     if (res >= 1) {
          send_buf(fail_user, sizeof(fail_user), clt_addr, clt_addr_len);
          return;
     }

     res = add_user(user.name, user.password, "", "", "", "", "", "", "", "", "", "", &user_id);
     printf("Result of adding a user was %d, user_id is %d\n",
            res, user_id);
     if (!res) {
          send_buf(fail, sizeof(fail), clt_addr, clt_addr_len);
          return;
     }

     s = put_register_ok_info(&wr_buf);
     if (send_buf(wr_buf, s, clt_addr, clt_addr_len) == -1)
          return;
     free(wr_buf);
}

static void handle_login(char *buf, size_t n,
                         char *host, char *service,
                         struct sockaddr_storage *clt_addr,
                         socklen_t clt_addr_len)
{
     int res;
     size_t s;
     char fail_pw[] = "wrong password";
     char fail_un[] = "no such user";
     char fail[] = "login save fail";
     char serv_name[32];
     char *wr_buf;
     struct user_st user;
     struct user_st result;

     get_login_info(buf, &user, serv_name);
     res = find_user(user.name, &result);
     if (res <= 0) {
          send_buf(fail_un, sizeof(fail_un), clt_addr, clt_addr_len);
          return;
     }

     if (strcmp(user.password, result.password)) {
          send_buf(fail_pw, sizeof(fail_pw), clt_addr, clt_addr_len);
          return;
     }

     if (!strcmp(serv_name, SERV_VRECV))
          res = update_user(result.id, result.name, result.password, host, service, result.port_vrecv_c, result.port_arecv, result.port_arecv_c, result.port_vsend, result.port_vsend_c, result.port_asend, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_VRECV_C))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, service, result.port_arecv, result.port_arecv_c, result.port_vsend, result.port_vsend_c, result.port_asend, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_ARECV))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, service, result.port_arecv_c, result.port_vsend, result.port_vsend_c, result.port_asend, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_ARECV_C))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, result.port_arecv, service, result.port_vsend, result.port_vsend_c, result.port_asend, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_VSEND))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, result.port_arecv, result.port_arecv_c, service, result.port_vsend_c, result.port_asend, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_VSEND_C))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, result.port_arecv, result.port_arecv_c, result.port_vsend, service, result.port_asend, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_ASEND))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, result.port_arecv, result.port_arecv_c, result.port_vsend, result.port_vsend_c, service, result.port_asend_c, result.port_toserver);
     else if (!strcmp(serv_name, SERV_ASEND_C))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, result.port_arecv, result.port_arecv_c, result.port_vsend, result.port_vsend_c, result.port_asend, service, result.port_toserver);
     else if (!strcmp(serv_name, SERV_TOSERVER))
          res = update_user(result.id, result.name, result.password, host, result.port_vrecv, result.port_vrecv_c, result.port_arecv, result.port_arecv_c, result.port_vsend, result.port_vsend_c, result.port_asend, result.port_asend_c, service);
     else {
          fprintf(stderr, "unrecognized service name: %s", serv_name);
          return;
     }
     if (!res) {
          send_buf(fail, sizeof(fail), clt_addr, clt_addr_len);
          return;
     }

     s = put_login_ok_info(serv_name, &wr_buf);
     if (send_buf(wr_buf, s, clt_addr, clt_addr_len) == -1)
          return;
     free(wr_buf);
}

static void handle_transmit(char *buf, size_t n,
                            struct sockaddr_storage *clt_addr,
                            socklen_t clt_addr_len)
{
     int res;
     size_t s;
     char from[MAX_USERNAME], to[MAX_USERNAME];
     char *wr_buf;
     char rd_buf[BUFSIZ];
     struct user_st sender, receiver;
     struct addrinfo recv_addr;
     char fail_user[] = "no such user";

     get_transmit_info(buf, from, to);

     res = find_user(to, &receiver);
     if (res <= 0) {
          send_buf(fail_user, sizeof(fail_user), clt_addr, clt_addr_len);
          return;
     }
     res = find_user(from, &sender);
     if (res <= 0) {
          send_buf(fail_user, sizeof(fail_user), clt_addr, clt_addr_len);
          return;
     }

     /* need timestamp */
     s = put_receive_info(from, to, &sender, &wr_buf);
     if (make_addr(receiver.host, receiver.port_toserver, &recv_addr) == -1) {
          perror("make_addr:");
          exit(EXIT_FAILURE);
     }
     s = send_and_recv(wr_buf, s, rd_buf, sfd, &recv_addr);
     if (!is_receive_ok(rd_buf, s)) {
          fprintf(stderr, "Error receive_fail\n");
          return;
     }

     s = put_transmit_ok_info(from, to, &receiver, &wr_buf);
     if (sendto(sfd, wr_buf, s, 0, (struct sockaddr *)clt_addr,
                clt_addr_len) != s) {
          fprintf(stderr, "Error sending response\n");
          return;
     }
     free(wr_buf);
}
