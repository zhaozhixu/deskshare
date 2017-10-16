#ifndef _DESKSHARE_H_
#define _DESKSHARE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pwd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef SERVER
#include <mysql.h>
#endif

#define DEFAULT_VRECV 5000
#define DEFAULT_ARECV 5002
#define DEFAULT_VSEND 5004
#define DEFAULT_ASEND 5006
#define DEFAULT_VRECV_C 5001
#define DEFAULT_ARECV_C 5003
#define DEFAULT_VSEND_C 5005
#define DEFAULT_ASEND_C 5007
#define DEFAULT_TOSERVER 5008
#define DEFAULT_SERVER_PORT 5009
#define DEFAULT_SERVER_HOST "139.199.163.114"

#define SERV_VRECV "VRECV"
#define SERV_ARECV "ARECV"
#define SERV_VSEND "VSEND"
#define SERV_ASEND "ASEND"
#define SERV_VRECV_C "VRECV_C"
#define SERV_ARECV_C "ARECV_C"
#define SERV_VSEND_C "VSEND_C"
#define SERV_ASEND_C "ASEND_C"
#define SERV_TOSERVER "TOSERVER"
#define MAX_USERNAME 32
#define MAX_PASSWORD 32

#ifdef WINDOWS
#define DELIMITER "\\"
#else
#define DELIMITER "/"
#endif

struct user_st {
     int id;
     char name[MAX_USERNAME];
     char password[MAX_PASSWORD];
     char host[NI_MAXHOST];
     char port_vrecv[NI_MAXSERV];
     char port_vrecv_c[NI_MAXSERV];
     char port_arecv[NI_MAXSERV];
     char port_arecv_c[NI_MAXSERV];
     char port_vsend[NI_MAXSERV];
     char port_vsend_c[NI_MAXSERV];
     char port_asend[NI_MAXSERV];
     char port_asend_c[NI_MAXSERV];
     char port_toserver[NI_MAXSERV];
};

#endif  /* _DESKSHARE_H_ */
