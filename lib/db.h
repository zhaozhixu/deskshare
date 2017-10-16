#include "deskshare.h"

#define MAX_SQL 1024

int db_start(char *name, char *password);
void db_end();

int add_user(char *name, char *password, char *host,
             char *port_vrecv, char *port_vrecv_c,
             char *port_arecv, char *port_arecv_c,
             char *port_vsend, char *port_vsend_c,
             char *port_asend, char *port_asend_c,
             char *port_toserver, int *user_id);
int find_user(char *name, struct user_st *result);
int update_user(int id, char *name, char *password, char *host,
                char *port_vrecv, char *port_vrecv_c,
                char *port_arecv, char *port_arecv_c,
                char *port_vsend, char *port_vsend_c,
                char *port_asend, char *port_asend_c,
                char *port_toserver);
int delete_user(int id);
