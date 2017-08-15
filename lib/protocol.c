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
     "Port: %d\r\n\r\n";

static char LOGIN_OK_FMT[] =
     "Ack: LOGIN_OK\r\n"
     "Port: %d\r\n\r\n";

static char TRANSMIT_FMT[] =
     "Req: TRANSMIT\r\n"
     "Peer: %s\r\n\r\n";

static char TRANSMIT_OK_FMT[] =
     "Ack: TRANSMIT_OK\r\n"
     "Host: %s\r\n"
     "Port_5000: %s\r\n"
     "Port_5002: %s\r\n\r\n";

static char RECEIVE_FMT[] =
     "Msg: RECEIVE\r\n"
     "Peer: %s\r\n\r\n";

static int digit_num(size_t n)
{
     int i;
     for (i = 1; n / 10; i++)
          n /= 10;
     return i;
}

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
     char msg[WORD_LENGTH], msg_arg[WORD_LENGTH];

     sscanf(buf, "%s%s", msg, msg_arg);
     if (strcmp(msg, "Msg:") || strcmp(msg_arg, "RECEIVE"))
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

size_t put_login_info(struct user_st *user, int port, char **buf)
{
     size_t n;

     n = sizeof(LOGIN_FMT) - 6 + strlen(user->name) +
          strlen(user->password) + digit_num(port);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_login_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, LOGIN_FMT, user->name, user->password, port);
     return n;
}

size_t put_login_ok_info(int port, char **buf)
{
     size_t n;

     n = sizeof(LOGIN_OK_FMT) -2 + digit_num(port);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_login_ok_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, LOGIN_OK_FMT, port);
     return n;
}

size_t put_transmit_info(char *peername, char **buf)
{
     size_t n;

     n = sizeof(TRANSMIT_FMT) - 2 + strlen(peername);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_transmit_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, TRANSMIT_FMT, peername);
     return n;
}

size_t put_transmit_ok_info(struct user_st *peer, char **buf)
{
     size_t n;

     n = sizeof(TRANSMIT_OK_FMT) - 6 + strlen(peer->host) +
          strlen(peer->port_5000), strlen(peer->port_5002);
     *buf = malloc(n);
     if (*buf == NULL) {
          perror("put_transmit_ok_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*buf, TRANSMIT_OK_FMT, peer->host, peer->port_5000,
             peer->port_5002);
     return n;
}

int get_register_info(char *buf, struct user_st *user)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH],
          name[WORD_LENGTH], password[WORD_LENGTH];

     sscanf(buf, "%s%s%s%s%s%s", req, req_arg, name, user->name,
            password, user->password);
     return 1;
}

int get_login_info(char *buf, struct user_st *user, int *port)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH],
          name[WORD_LENGTH], password[WORD_LENGTH],
          port_token[WORD_LENGTH], port_arg[WORD_LENGTH];

     sscanf(buf, "%s%s%s%s%s%s%s%s", req, req_arg, name, user->name,
            password, user->password, port_token, port_arg);
     *port = atoi(port_arg);
     return 1;
}
int get_login_ok_info(char *buf, int *port)
{
     char ack[WORD_LENGTH], ack_arg[WORD_LENGTH],
          port_token[WORD_LENGTH], port_arg[WORD_LENGTH];

     sscanf(buf, "%s%s%s%s", ack, ack_arg, port_token, port_arg);
     *port = atoi(port_arg);
     return 1;
}

int get_transmit_info(char *buf, char **peername)
{
     char req[WORD_LENGTH], req_arg[WORD_LENGTH],
          peer[WORD_LENGTH];

     *peername = malloc(WORD_LENGTH);
     if (*peername == NULL) {
          perror("get_transmit_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sscanf(buf, "%s%s%s%s", req, req_arg, peer, *peername);
     return 1;
}

int get_transmit_ok_info(char *buf, struct user_st *peer)
{
     char ack[WORD_LENGTH], ack_arg[WORD_LENGTH],
          host[WORD_LENGTH], port_5000[WORD_LENGTH],
          port_5002[WORD_LENGTH];

     sscanf(buf, "%s%s%s%s%s%s%s%s", ack, ack_arg, host, peer->host,
            port_5000, peer->port_5000, port_5002, peer->port_5002);
     return 1;
}

int get_receive_info(char *buf, char **peername)
{
     char msg[WORD_LENGTH], msg_arg[WORD_LENGTH],
          peer[WORD_LENGTH];

     *peername = malloc(WORD_LENGTH);
     if (*peername == NULL) {
          perror("get_transmit_info: error malloc");
          exit(EXIT_FAILURE);
     }

     sscanf(buf, "%s%s%s%s", msg, msg_arg, peer, *peername);
     return 1;
}
