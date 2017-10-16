#include "deskshare.h"

int is_register(char *buf, size_t n);
int is_login(char *buf, size_t n);
int is_transmit(char *buf, size_t n);
int is_receive(char *buf, size_t n);
int is_register_ok(char *buf, size_t n);
int is_login_ok(char *buf, size_t n);
int is_transmit_ok(char *buf, size_t n);
int is_receive_ok(char *buf, size_t n);

size_t put_register_info(struct user_st *user, char **buf);
size_t put_register_ok_info(char **buf);
size_t put_login_info(struct user_st *user, char *port, char **buf);
size_t put_login_ok_info(char *port, char **buf);
size_t put_transmit_info(char *from, char *to, char **buf);
size_t put_transmit_ok_info(char *from, char *to,
                            struct user_st *peer, char **buf);
size_t put_receive_info(char *from, char *to,
                        struct user_st *peer, char **buf);
size_t put_receive_ok_info(char *from, char *to, char **buf);

int get_register_info(char *buf, struct user_st *user);
int get_login_info(char *buf, struct user_st *user, char *port);
int get_login_ok_info(char *buf, char *port);
int get_transmit_info(char *buf, char *from, char *to);
int get_transmit_ok_info(char *buf, char *from, char *to,
                         struct user_st *peer);
int get_receive_info(char *buf, char *from, char *to,
                     struct user_st *peer);
int get_receive_ok_info(char *buf, char *from, char *to);
