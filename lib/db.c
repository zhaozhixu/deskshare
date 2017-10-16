#include "db.h"

static MYSQL my_connection;
static int dbconnected = 0;

int db_start(char *name, char *password) {
     if (dbconnected)
          return 1;

     mysql_init(&my_connection);
     if (!mysql_real_connect(&my_connection, "localhost", name,
                             password, "deskshare", 0, NULL, 0)) {
          fprintf(stderr, "Database connection failure: %d, %s\n",
                  mysql_errno(&my_connection),
                  mysql_error(&my_connection));
          return 0;
     }

     dbconnected = 1;
     return 1;
}

void db_end() {
     if (dbconnected)
          mysql_close(&my_connection);
     dbconnected = 0;
}

int add_user(char *name, char *password, char *host,
             char *port_vrecv, char *port_vrecv_c,
             char *port_arecv, char *port_arecv_c,
             char *port_vsend, char *port_vsend_c,
             char *port_asend, char *port_asend_c,
             char *port_toserver, int *user_id)
{
     MYSQL_RES *res_ptr;
     MYSQL_ROW mysqlrow;

     int res;
     char is[250];
     char es[250];
     int new_user_id = -1;

     if (!dbconnected)
          return 0;

     mysql_escape_string(es, name, strlen(name));
     sprintf(is, "INSERT INTO user(name, password, host,"
             "port_vrecv,  port_vrecv_c, port_arecv,  port_arecv_c,"
             "port_vsend,  port_vsend_c, port_asend,  port_asend_c,"
             "port_toserver)"
             "VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s',"
             "'%s','%s','%s','%s','%s')",
             name, password, host,
             port_vrecv,  port_vrecv_c, port_arecv,  port_arecv_c,
             port_vsend,  port_vsend_c, port_asend,  port_asend_c,
             port_toserver);

     res = mysql_query(&my_connection, is);
     if (res) {
          fprintf(stderr, "Insert error %d: %s\n",
                  mysql_errno(&my_connection),
                  mysql_error(&my_connection));
          return 0;
     }

     res = mysql_query(&my_connection, "SELECT LAST_INSERT_ID()");
     if (res) {
          fprintf(stderr, "SELECT error: %s\n",
                  mysql_error(&my_connection));
          return 0;
     } else {
          res_ptr = mysql_use_result(&my_connection);
          if (res_ptr) {
               if ((mysqlrow = mysql_fetch_row(res_ptr))) {
                    sscanf(mysqlrow[0], "%d", &new_user_id);
               }
               mysql_free_result(res_ptr);
          }
     }

     *user_id = new_user_id;
     if (new_user_id != -1)
          return 1;
     return 0;
}

int find_user(char *name, struct user_st *result) {
     MYSQL_RES *res_ptr;
     MYSQL_ROW mysqlrow;

     int res;
     char qs[500];
     char ss[250];

     int num_rows = 0;

     if (!dbconnected)
          return 0;

     memset(result, -1, sizeof(*result));
     mysql_escape_string(ss, name, strlen(name));

     sprintf(qs, "SELECT * FROM user WHERE name='%s'", name);

     res = mysql_query(&my_connection, qs);
     if (res) {
          fprintf(stderr, "SELECT error: %s\n",
                  mysql_error(&my_connection));
          return 0;
     } else {
          res_ptr = mysql_store_result(&my_connection);
          if (res_ptr) {
               num_rows = mysql_num_rows(res_ptr);
               if (num_rows > 0) {
                    while (mysqlrow = mysql_fetch_row(res_ptr)) {
                         sscanf(mysqlrow[0], "%d", &result->id);
                         strcpy(result->name, mysqlrow[1]);
                         strcpy(result->password, mysqlrow[2]);
                         strcpy(result->host, mysqlrow[3]);
                         strcpy(result->port_vrecv, mysqlrow[4]);
                         strcpy(result->port_vrecv_c, mysqlrow[5]);
                         strcpy(result->port_arecv, mysqlrow[6]);
                         strcpy(result->port_arecv_c, mysqlrow[7]);
                         strcpy(result->port_vsend, mysqlrow[8]);
                         strcpy(result->port_vsend_c, mysqlrow[9]);
                         strcpy(result->port_asend, mysqlrow[10]);
                         strcpy(result->port_asend_c, mysqlrow[11]);
                         strcpy(result->port_toserver, mysqlrow[12]);
                    }
               }
          }
          mysql_free_result(res_ptr);
     }
     return num_rows;
}

int delete_user(int id) {
     int res;
     char qs[250];
     int num_rows;
     MYSQL_RES *res_ptr;
     MYSQL_ROW mysqlrow;

     if (!dbconnected)
          return 0;

     sprintf(qs, "DELETE FROM user where id = '%d'", id);
     res = mysql_query(&my_connection, qs);
     if (res) {
          fprintf(stderr, "Delete error (user) %d: %s\n",
                  mysql_errno(&my_connection),
                  mysql_error(&my_connection));
          return 0;
     }

     return 1;
}

int update_user(int id, char *name, char *password, char *host,
                char *port_vrecv, char *port_vrecv_c,
                char *port_arecv, char *port_arecv_c,
                char *port_vsend, char *port_vsend_c,
                char *port_asend, char *port_asend_c,
                char *port_toserver)
{
     int res;
     char qs[250];

     sprintf(qs, "UPDATE user SET name = '%s', password = '%s',"
             "host = '%s', port_vrecv = '%s', port_vrecv_c = '%s',"
             "port_arecv = '%s', port_arecv_c = '%s',"
             "port_vsend = '%s', port_vsend_c = '%s',"
             "port_asend = '%s', port_asend_c = '%s',"
             "port_toserver = '%s' WHERE id = '%d'",
             name, password, host, port_vrecv, port_vrecv_c,
             port_arecv, port_arecv_c, port_vsend, port_vsend_c,
             port_asend, port_asend_c, port_toserver, id);

     res = mysql_query(&my_connection, qs);
     if (res) {
          fprintf(stderr, "UPDATE error: %s\n",
                  mysql_error(&my_connection));
          return 0;
     }

     return 1;
}
