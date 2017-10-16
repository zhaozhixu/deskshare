#include "protocol.h"

#define WORD_LENGTH 32

static char REGISTER_FMT[] =
     "Req: REGISTER\r\n"
     "Name: %s\r\n"
     "Password: %s\r\n\r\n";

static char REGISTER_OK[] =
     "Ack: REGISTER_OK\r\n\r\n";

static char LOGIN_FMT[] =
     "Req: LOGIN\r\n"
     "Name: %s\r\n"
     "Password: %s\r\n"
     "Port: %s\r\n\r\n";

static char LOGIN_OK_FMT[] =
     "Ack: LOGIN_OK\r\n"
     "Port: %s\r\n\r\n";

static char TRANSMIT_FMT[] =
     "Req: TRANSMIT\r\n"
     "From: %s\r\n"
     "To: %s\r\n\r\n";

static char TRANSMIT_OK_FMT[] =
     "Ack: TRANSMIT_OK\r\n"
     "From: %s\r\n"
     "To: %s\r\n"
     "Host: %s\r\n"
     "Port_vrecv: %s\r\n"
     "Port_vrecv_c: %s\r\n"
     "Port_arecv: %s\r\n"
     "Port_arecv_c: %s\r\n\r\n";

static char RECEIVE_FMT[] =
     "Req: RECEIVE\r\n"
     "From: %s\r\n"
     "To: %s\r\n"
     "Host: %s\r\n"
     "Port_vsend: %s\r\n"
     "Port_vsend_c: %s\r\n"
     "Port_asend: %s\r\n"
     "Port_asend_c: %s\r\n\r\n";

static char RECEIVE_OK_FMT[] =
     "Ack: RECEIVE_OK\r\n"
     "From: %s\r\n"
     "To: %s\r\n\r\n";

/* static int digit_num(size_t n) */
/* { */
/*      int i; */
/*      for (i = 1; n / 10; i++) */
/*           n /= 10; */
/*      return i; */
/* } */

int is_register(char *buf, size_t n)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", req, req_arg);
     if (strcmp(req, "Req:") || strcmp(req_arg, "REGISTER"))
          return 0;
     return 1;
}

int is_login(char* buf, size_t n)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", req, req_arg);
     if (strcmp(req, "Req:") || strcmp(req_arg, "LOGIN"))
          return 0;
     return 1;
}

int is_transmit(char* buf, size_t n)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", req, req_arg);
     if (strcmp(req, "Req:") || strcmp(req_arg, "TRANSMIT"))
          return 0;
     return 1;
}

int is_receive(char *buf, size_t n)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", req, req_arg);
     if (strcmp(req, "Req:") || strcmp(req_arg, "RECEIVE"))
          return 0;
     return 1;
}

int is_register_ok(char *buf, size_t n)
{
     char ack[WORD_LENGTH], ack_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", ack, ack_arg);
     if (strcmp(ack, "Ack:") || strcmp(ack_arg, "REGISTER_OK"))
          return 0;
     return 1;
}

int is_login_ok(char *buf, size_t n)
{
     char ack[WORD_LENGTH], ack_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", ack, ack_arg);
     if (strcmp(ack, "Ack:") || strcmp(ack_arg, "LOGIN_OK"))
          return 0;
     return 1;
}

int is_transmit_ok(char *buf, size_t n)
{
     char ack[WORD_LENGTH], ack_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", ack, ack_arg);
     if (strcmp(ack, "Ack:") || strcmp(ack_arg, "TRANSMIT_OK"))
          return 0;
     return 1;
}

int is_receive_ok(char *buf, size_t n)
{
     char ack[WORD_LENGTH], ack_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", ack, ack_arg);
     if (strcmp(ack, "Ack:") || strcmp(ack_arg, "RECEIVE_OK"))
          return 0;
     return 1;
}

size_t put_register_info(struct user_st *user, char **buf)
{
     size_t n;

     n = sizeof(REGISTER_FMT) - 4 + strlen(user->name) +
          strlen(user->password);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_register_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, REGISTER_FMT, user->name, user->password);
     return n;
}

size_t put_register_ok_info(char **buf)
{
     size_t n;

     n = sizeof(REGISTER_OK);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_register_ok_info: error malloc");
          exit(EXIT_FAILURE);
     }

     strcpy(*buf, REGISTER_OK);
     return n;
}

size_t put_login_info(struct user_st *user, char *port, char **buf)
{
     size_t n;

     n = sizeof(LOGIN_FMT) - 6 + strlen(user->name) +
          strlen(user->password) + strlen(port);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_login_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, LOGIN_FMT, user->name, user->password, port);
     return n;
}

size_t put_login_ok_info(char *port, char **buf)
{
     size_t n;

     n = sizeof(LOGIN_OK_FMT) -2 + strlen(port);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_login_ok_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, LOGIN_OK_FMT, port);
     return n;
}

size_t put_transmit_info(char *from, char *to, char **buf)
{
     size_t n;

     n = sizeof(TRANSMIT_FMT) - 4 + strlen(from) + strlen(to);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_transmit_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, TRANSMIT_FMT, from, to);
     return n;
}

size_t put_transmit_ok_info(char *from, char *to,
                            struct user_st *peer, char **buf)
{
     size_t n;

     n = sizeof(TRANSMIT_OK_FMT) - 14 + strlen(from) + strlen(to) +
          strlen(peer->host) + strlen(peer->port_vrecv) +
          strlen(peer->port_vrecv_c) + strlen(peer->port_arecv) +
          strlen(peer->port_arecv_c);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_transmit_ok_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, TRANSMIT_OK_FMT, from, to, peer->host,
             peer->port_vrecv, peer->port_vrecv_c,
             peer->port_arecv, peer->port_arecv_c);
     return n;
}

size_t put_receive_info(char *from, char *to,
                        struct user_st *peer, char **buf)
{
     size_t n;

     n = sizeof(RECEIVE_FMT) - 14 + strlen(from) + strlen(to) +
          strlen(peer->host) + strlen(peer->port_vsend) +
          strlen(peer->port_vsend_c) + strlen(peer->port_asend) +
          strlen(peer->port_asend_c);
     *buf = malloc(n);

     if (*buf == NULL) {
          perror("put_receive_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, RECEIVE_FMT, from, to, peer->host,
             peer->port_vsend, peer->port_vsend_c,
             peer->port_asend, peer->port_asend_c);
     return n;
}

size_t put_receive_ok_info(char *from, char *to, char **buf)
{
     size_t n;

     n = sizeof(RECEIVE_OK_FMT) - 4 + strlen(from) + strlen(to);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_receive_ok_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, RECEIVE_OK_FMT, from, to);
     return n;
}

int get_register_info(char *buf, struct user_st *user)
{
     sscanf(buf, REGISTER_FMT, user->name, user->password);
     return 1;
}

int get_login_info(char *buf, struct user_st *user, char *port)
{
     sscanf(buf, LOGIN_FMT, user->name, user->password, port);
     return 1;
}
int get_login_ok_info(char *buf, char *port)
{
     sscanf(buf, LOGIN_OK_FMT, port);
     return 1;
}

int get_transmit_info(char *buf, char *from, char *to)
{
     sscanf(buf, TRANSMIT_FMT, from, to);
     return 1;
}

int get_transmit_ok_info(char *buf, char *from, char *to,
                         struct user_st *peer)
{
     sscanf(buf, TRANSMIT_OK_FMT, from, to, peer->host,
            peer->port_vrecv, peer->port_vrecv_c,
            peer->port_arecv, peer->port_arecv_c);
     return 1;
}

int get_receive_info(char *buf, char *from, char *to,
                     struct user_st *peer)
{
     sscanf(buf, RECEIVE_FMT, from, to, peer->host,
            peer->port_vsend, peer->port_vsend_c,
            peer->port_asend, peer->port_asend_c);
     return 1;
}

int get_receive_ok_info(char *buf, char *from, char *to)
{
     sscanf(buf, RECEIVE_OK_FMT, from, to);
     return 1;
}
