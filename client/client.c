#include "../lib/dgram.h"
#include "../lib/protocol.h"
#include "../lib/dsclient.h"
#include "../lib/dsserver.h"

#define RWRWR (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH)
#define RWXRWXRX (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define CFG_NUM 11

static void init();
static void handle_register(char *username, char *password);
static void handle_login(char *username, char *password);
static void handle_transmit(char *name, char *peername);
static void handle_receive();

static int port_vrecv, port_arecv, port_vsend, port_asend,
     port_vrecv_c, port_arecv_c, port_vsend_c, port_asend_c,
     port_toserver, serverport;
static char *serverhost;
static struct addrinfo serveraddr;
static int sfd_vrecv, sfd_arecv, sfd_vsend, sfd_asend,
     sfd_vrecv_c, sfd_arecv_c, sfd_vsend_c, sfd_asend_c,
     sfd_toserver;

static char cfg_fmt[] =
     "serverhost=%s\n"
     "serverport=%d\n"
     "port_vrecv=%d\n"
     "port_vrecv_c=%d\n"
     "port_arecv=%d\n"
     "port_arecv_c=%d\n"
     "port_vsend=%d\n"
     "port_vsend_c=%d\n"
     "port_asend=%d\n"
     "port_asend_c=%d\n"
     "port_toserver=%d\n";

static char usage[] =
     "usage: deskshare <command> [<args>]\n"
     "\tregister <username> <password> : register a new user\n"
     "\tlogin <username> <password> : refresh your host and ports\n"
     "\ttransmit <username> <peername> : transmit video to a peer\n"
     "\treceive <username>: wait for a call\n";

int main(int argc, char *argv[])
{
     if (argc < 2) {
          printf("%s", usage);
          exit(EXIT_SUCCESS);
     }

     init();

     if (!strcmp(argv[1], "register"))
          handle_register(argv[2], argv[3]);
     else if (!strcmp(argv[1], "login"))
          handle_login(argv[2], argv[3]);
     else if (!strcmp(argv[1], "transmit"))
          handle_transmit(argv[2], argv[3]);
     else if (!strcmp(argv[1], "receive"))
          handle_receive();
     else
          printf("%s", usage);

     exit(EXIT_SUCCESS);
}

static void default_config()
{
     port_vrecv = DEFAULT_VRECV;
     port_arecv = DEFAULT_ARECV;
     port_vsend = DEFAULT_VSEND;
     port_asend = DEFAULT_ASEND;
     port_vrecv_c = DEFAULT_VRECV_C;
     port_arecv_c = DEFAULT_ARECV_C;
     port_vsend_c = DEFAULT_VSEND_C;
     port_asend_c = DEFAULT_ASEND_C;
     port_toserver = DEFAULT_TOSERVER;
     serverport = DEFAULT_SERVER_PORT;
     if (!serverhost && !(serverhost = malloc(NI_MAXHOST))) {
          perror("malloc");
          exit(EXIT_FAILURE);
     }
     strcpy(serverhost, DEFAULT_SERVER_HOST);
}

static void load_config()
{
     char cfg_fmt[] =
          "serverhost=%s\n"
          "serverport=%d\n"
          "port_vrecv=%d\n"
          "port_vrecv_c=%d\n"
          "port_arecv=%d\n"
          "port_arecv_c=%d\n"
          "port_vsend=%d\n"
          "port_vsend_c=%d\n"
          "port_asend=%d\n"
          "port_asend_c=%d\n"
          "port_toserver=%d\n";
     char *cfg_dir = ".deskshare";
     char *cfg_name = "dsconfig";
     char *cfg_path, *home_path;
     char buf[BUFSIZ], *cfg_str = NULL;
     int fd;
     FILE *file;
     DIR *dp;
     size_t n, sn = 0, size = 0;
     struct stat cfg_stat;

     home_path = getpwuid(getuid())->pw_dir;
     cfg_path = malloc(strlen(home_path) + strlen(cfg_dir) + strlen(cfg_name) + 3);
     if (!cfg_path) {
          perror("malloc");
          exit(EXIT_FAILURE);
     }
     strcpy(cfg_path, home_path);
     strcat(cfg_path, DELIMITER);
     strcat(cfg_path, cfg_dir);

     if (!(dp = opendir(cfg_path)) && errno == ENOENT && mkdir(cfg_path, RWXRWXRX)) {
          perror("mkdir");
          fprintf(stderr, "use default config\n");
          default_config();
          free(cfg_path);
          return;
     }
     strcat(cfg_path, DELIMITER);
     strcat(cfg_path, cfg_name);

     fd = open(cfg_path, O_RDWR|O_CREAT, RWRWR);
     fstat(fd, &cfg_stat);
     if (cfg_stat.st_size == 0) {
          file = fdopen(fd, "r+");
          fprintf(file, cfg_fmt, DEFAULT_SERVER_HOST,
                  DEFAULT_SERVER_PORT, DEFAULT_VRECV,
                  DEFAULT_VRECV_C, DEFAULT_ARECV,
                  DEFAULT_ARECV_C, DEFAULT_VSEND,
                  DEFAULT_VSEND_C, DEFAULT_ASEND,
                  DEFAULT_ASEND_C, DEFAULT_TOSERVER);
          default_config();
          free(cfg_path);
          fclose(file);
          closedir(dp);
          return;
     }

     while ((n = read(fd, buf, BUFSIZ)) > 0) {
          if (sn + n > size) {
               if (!(cfg_str = realloc(cfg_str, size + BUFSIZ))) {
                    perror("realloc");
                    exit(EXIT_FAILURE);
               }
               size += BUFSIZ;
          }
          memmove(cfg_str + sn, buf, n);
          sn += n;
     }
     if (!serverhost && !(serverhost = malloc(NI_MAXHOST))) {
          perror("malloc");
          exit(EXIT_FAILURE);
     }
     n = sscanf(cfg_str, cfg_fmt, serverhost,
                &serverport, &port_vrecv,
                &port_vrecv_c, &port_arecv,
                &port_arecv_c, &port_vsend,
                &port_vsend_c, &port_asend,
                &port_asend_c, &port_toserver);
     if (n != CFG_NUM) {
          fprintf(stderr, "wrong config file format, use default\n");
          default_config();
          free(cfg_path);
          free(cfg_str);
          close(fd);
          closedir(dp);
          return;
     }

     free(cfg_path);
     free(cfg_str);
     close(fd);
     closedir(dp);
}

static void init()
{
     char service[NI_MAXSERV];

     load_config();
#ifdef DEBUG
     fprintf(stderr, cfg_fmt, serverhost,
            serverport, port_vrecv,
            port_vrecv_c, port_arecv,
            port_arecv_c, port_vsend,
            port_vsend_c, port_asend,
            port_asend_c, port_toserver);
#endif
     if ((sfd_vrecv = make_dgram_server_socket(port_vrecv)) == -1
         || (sfd_arecv = make_dgram_server_socket(port_arecv)) == -1
         || (sfd_vsend = make_dgram_server_socket(port_vsend)) == -1
         || (sfd_asend = make_dgram_server_socket(port_asend)) == -1
         || (sfd_vrecv_c = make_dgram_server_socket(port_vrecv_c)) == -1
         || (sfd_arecv_c = make_dgram_server_socket(port_arecv_c)) == -1
         || (sfd_vsend_c = make_dgram_server_socket(port_vsend_c)) == -1
         || (sfd_asend_c = make_dgram_server_socket(port_asend_c)) == -1
         || (sfd_toserver = make_dgram_server_socket(port_toserver)) == -1)
          exit(EXIT_FAILURE);

     sprintf(service, "%d", serverport);
     if(make_addr(serverhost, service, &serveraddr) == -1) {
          perror("make_addr:");
          exit(EXIT_FAILURE);
     }
}

static void handle_register(char *username, char *password)
{
     struct user_st user;
     char *wr_buf;
     char rd_buf[BUFSIZ];
     size_t s;

     strcpy(user.name, username);
     strcpy(user.password, password);
     s = put_register_info(&user, &wr_buf);
     s = send_and_recv(wr_buf, s, rd_buf, sfd_toserver, &serveraddr);
     free(wr_buf);
     if (!is_register_ok(rd_buf, s)) {
          printf("%s", rd_buf);
          return;
     }
     printf("Register success!\n");
}

static int login_service(struct user_st *user, char *service, int sfd)
{
     char *wr_buf;
     char rd_buf[BUFSIZ];
     size_t s;

     s = put_login_info(user, service, &wr_buf);
     s = send_and_recv(wr_buf, s, rd_buf, sfd, &serveraddr);
     free(wr_buf);
     if (!is_login_ok(rd_buf, s)) {
          fprintf(stderr, "%s", rd_buf);
          return -1;
     }

     return 0;
}

static void handle_login(char *username, char *password)
{
     struct user_st user;

     strcpy(user.name, username);
     strcpy(user.password, password);

     if ((login_service(&user, SERV_VRECV, sfd_vrecv)) == -1
         || (login_service(&user, SERV_ARECV, sfd_arecv) == -1)
         || (login_service(&user, SERV_VSEND, sfd_vsend)) == -1
         || (login_service(&user, SERV_ASEND, sfd_asend)) == -1
         || (login_service(&user, SERV_VRECV_C, sfd_vrecv_c)) == -1
         || (login_service(&user, SERV_ARECV_C, sfd_arecv_c)) == -1
         || (login_service(&user, SERV_VSEND_C, sfd_vsend_c)) == -1
         || (login_service(&user, SERV_ASEND_C, sfd_asend_c)) == -1
         || (login_service(&user, SERV_TOSERVER, sfd_toserver)) == -1) {
          fprintf(stderr, "Login failed.\n");
          return;
     }

     printf("Login success.\n");
}

static void handle_transmit(char *name, char *peername)
{
     struct user_st peer;
     char *wr_buf;
     char rd_buf[BUFSIZ];
     char from[MAX_USERNAME], to[MAX_USERNAME];
     size_t s;

     s = put_transmit_info(name, peername, &wr_buf);
     s = send_and_recv(wr_buf, s, rd_buf, sfd_toserver, &serveraddr);
     free(wr_buf);
     if (!is_transmit_ok(rd_buf, s)) {
          printf("%s", rd_buf);
          return;
     }

     get_transmit_ok_info(rd_buf, from, to, &peer);
     printf("transmit video to %s:%s, audio to %s:%s.\n", peer.host, peer.port_vrecv,
            peer.host, peer.port_arecv);
     transmitter_setup(peer.host, atoi(peer.port_vrecv), atoi(peer.port_arecv));
}

static int send_buf(char *host, char *service, int sfd, char *buf, size_t len)
{
     struct addrinfo addr;

     if (make_addr(host, service, &addr) == -1) {
          perror("make_addr:");
          return -1;
     }
     if (sendto(sfd, buf, len, 0, (struct sockaddr *)addr.ai_addr, addr.ai_addrlen) != len) {
          perror("sendto peer: Error sending response");
          return -1;
     }

     return 0;
}

static void handle_receive()
{
     struct user_st sender;
     size_t s;
     char rd_buf[BUFSIZ];
     char *wr_buf;
     char from[MAX_USERNAME], to[MAX_USERNAME];
     char buf[] = "something";
     int len = sizeof(buf);

     s = recvfrom(sfd_toserver, rd_buf, BUFSIZ, 0, NULL, NULL);
     if (s == -1) {
          perror("recvfrom");
          exit(EXIT_FAILURE);
     }
     get_receive_info(rd_buf, from, to, &sender);

     if ((send_buf(sender.host, sender.port_vsend, sfd_vrecv, buf, len)) == -1 || send_buf(sender.host, sender.port_asend, sfd_arecv, buf, len) == -1)
          exit(EXIT_FAILURE);

     s = put_receive_ok_info(to, from, &wr_buf);
     if (sendto(sfd_toserver, wr_buf, s, 0, (struct sockaddr *) serveraddr.ai_addr, serveraddr.ai_addrlen) != s) {
          perror("sendto server: Error sending response");
          exit(EXIT_FAILURE);
     }
     free(wr_buf);

     receiver_setup();
}
