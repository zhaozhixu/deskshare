#include <mysql.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_USERNAME 32
#define MAX_PASSWORD 32

struct user_st {
     int id;
     char name[MAX_USERNAME];
     char password[MAX_PASSWORD];
     char host[NI_MAXHOST];
     char port_5000[NI_MAXSERV];
     char port_5002[NI_MAXSERV];
};

int db_start(char *name, char *password);
void db_end();

int add_user(char *name, char *password, char *host,
             char *port_5000, char *port_5002, int *user_id);
int find_user(char *name, struct user_st *result);
int update_user(int id, char *name, char *password, char *host,
                char *port_5000, char *port_5002);
int delete_user(int id);
