#include <limo.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define INS_SOCKET_BUILTIN(f, name) setq(limo_socket_env, make_sym(name), make_builtin(f))
#define INS_SOCKET_VAR(val, name)   setq(limo_socket_env, make_sym(name), val)

// bind
// gethostbyname
// getpeername
// getsockname (?)
// setsockopt

limo_data *sym_socket;
limo_data *sym_sockaddr;

// domain: ex: AF_INET,
// type: ex: SOCK_STREAM,
// protocol: ex: usually 0
BUILTIN(builtin_socket_make_socket)
{
  limo_data *ld_domain, *ld_type, *ld_protocol;
  int domain, type, protocol;
  
  REQUIRE_ARGC("MAKE-SOCKET", 3);
  ld_domain = eval(FIRST_ARG, env);
  ld_type = eval(SECOND_ARG, env);
  ld_protocol = eval(THIRD_ARG, env);
  REQUIRE_TYPE("MAKE-SOCKET", ld_domain, limo_TYPE_GMPQ);
  REQUIRE_TYPE("MAKE-SOCKET", ld_type, limo_TYPE_GMPQ);
  REQUIRE_TYPE("MAKE-SOCKET", ld_protocol, limo_TYPE_GMPQ);
  domain   = GETINTFROMMPQ(ld_domain);
  type     = GETINTFROMMPQ(ld_type);
  protocol = GETINTFROMMPQ(ld_protocol);
  return make_number_from_long_long(socket(domain, type, protocol));
}

BUILTIN(builtin_socket_listen)
{
  limo_data *ld_sockfd, *ld_backlog;
  REQUIRE_ARGC("SOCKET-LISTEN", 2);
  ld_sockfd  = eval(FIRST_ARG, env);
  ld_backlog = eval(SECOND_ARG, env);
  REQUIRE_TYPE("SOCKET-LISTEN", ld_sockfd, limo_TYPE_GMPQ);
  REQUIRE_TYPE("SOCKET-LISTEN", ld_backlog, limo_TYPE_GMPQ);

  if (0>listen(GETINTFROMMPQ(ld_sockfd), GETINTFROMMPQ(ld_backlog)))
    limo_error_errno(sym_socket);

  return nil;
}

BUILTIN(builtin_socket_accept)
{
  limo_data *ld_sockfd;
  int res;

  REQUIRE_ARGC("SOCKET-ACCEPT", 1);
  ld_sockfd = eval(FIRST_ARG, env);
  REQUIRE_TYPE("SOCKET-ACCEPT", ld_sockfd, limo_TYPE_GMPQ);

  res = accept(GETINTFROMMPQ(ld_sockfd), NULL, NULL);
  if (0>res)
    limo_error_errno(sym_socket);

  return make_number_from_long_long(res);
}

BUILTIN(builtin_socket_make_sockaddr)
{
  struct sockaddr_in *sa;
  limo_data *ld_ip, *ld_port;
  in_addr_t ip; unsigned port;

  REQUIRE_ARGC("MAKE-SOCKADDR", 2);
  ld_ip = eval(FIRST_ARG, env);
  ld_port = eval(SECOND_ARG, env);
  REQUIRE_TYPE("MAKE-SOCKADDR", ld_ip, limo_TYPE_GMPQ);
  REQUIRE_TYPE("MAKE-SOCKADDR", ld_port, limo_TYPE_GMPQ);

  sa = GC_malloc_atomic(sizeof (struct sockaddr_in));
  port = GETINTFROMMPQ(ld_port);
  ip = GETINTFROMMPQ(ld_ip);
	
  sa->sin_family = AF_INET;
  sa->sin_port   = htons(port); // this is definitely correct, we couldn't connect otherwise.
  sa->sin_addr.s_addr = ip;
  
  return make_special(sym_sockaddr, sa);
}

BUILTIN(builtin_socket_inet_addr)
{
  limo_data *ld_in;
  REQUIRE_ARGC("INET-ADDR", 1);
  ld_in = eval(FIRST_ARG, env);
  REQUIRE_TYPE("INET-ADDR", ld_in, limo_TYPE_STRING);
  return make_number_from_long_long(inet_addr(ld_in->data.d_string));
}

BUILTIN(builtin_socket_connect)
{
  limo_data *ld_sockfd, *ld_sockaddr;
  int sockfd;
  struct sockaddr_in *sa;
  
  REQUIRE_ARGC("CONNECT", 2);
  ld_sockfd = eval(FIRST_ARG, env);
  ld_sockaddr = eval(SECOND_ARG, env);
  sa = get_special(ld_sockaddr, sym_sockaddr);
  REQUIRE_TYPE("CONNECT", ld_sockfd, limo_TYPE_GMPQ);
  sockfd = GETINTFROMMPQ(ld_sockfd);
  if (0>connect(sockfd, (struct sockaddr *)sa, sizeof *sa))
    limo_error_errno(sym_socket);
  return nil;
}

BUILTIN(builtin_socket_bind)
{
  limo_data *ld_sockfd, *ld_sockaddr;
  int sockfd;
  struct sockaddr_in *sa;

  REQUIRE_ARGC("BIND", 2);
  ld_sockfd = eval(FIRST_ARG, env);
  ld_sockaddr = eval(SECOND_ARG, env);
  sa = get_special(ld_sockaddr, sym_sockaddr);
  REQUIRE_TYPE("BIND", ld_sockfd, limo_TYPE_GMPQ);
  sockfd = GETINTFROMMPQ(ld_sockfd);
  if (0>bind(sockfd, (struct sockaddr *)sa, sizeof *sa))
    limo_error_errno(sym_socket);
  return nil;  
}

BUILTIN(builtin_socket_gethostbyname)
{
  struct hostent *he;
  limo_data *ld_hostname, *ld_res;

  REQUIRE_ARGC("GETHOSTBYNAME", 1);
  ld_hostname = eval(FIRST_ARG, env);
  REQUIRE_TYPE("GETHOSTBYNAME", ld_hostname, limo_TYPE_STRING);

  he = gethostbyname(ld_hostname->data.d_string);

  if (!he) throw(make_cons(sym_socket, make_string("couldn't resolve hostname")));

  ld_res = make_number_from_long_long(*(in_addr_t *)(he->h_addr));
  return ld_res;
}

void limo_init_socket(limo_data *env)
{
  limo_data *limo_socket_env;
  sym_socket = make_sym("SOCKET");
  sym_sockaddr = make_sym("SOCKADDR");

  limo_socket_env = make_env(nil);
  
  INS_SOCKET_BUILTIN(builtin_socket_make_socket, "MAKE-SOCKET");
  INS_SOCKET_BUILTIN(builtin_socket_listen, "LISTEN");
  INS_SOCKET_BUILTIN(builtin_socket_accept, "ACCEPT");
  INS_SOCKET_BUILTIN(builtin_socket_inet_addr, "INET-ADDR");
  INS_SOCKET_BUILTIN(builtin_socket_make_sockaddr, "MAKE-SOCKADDR");
  INS_SOCKET_BUILTIN(builtin_socket_connect, "CONNECT");
  INS_SOCKET_BUILTIN(builtin_socket_bind, "BIND");
  INS_SOCKET_BUILTIN(builtin_socket_gethostbyname, "GETHOSTBYNAME");

  INS_SOCKET_VAR(make_number_from_long_long(AF_INET), "AF_INET");
  INS_SOCKET_VAR(make_number_from_long_long(SOCK_STREAM), "SOCK_STREAM");
  INS_SOCKET_VAR(make_number_from_long_long(SOCK_DGRAM), "SOCK_DGRAM");
  INS_SOCKET_VAR(make_number_from_long_long(SOCK_NONBLOCK), "SOCK_NONBLOCK");
  
  setq(env, make_sym("_SOCKET"), limo_socket_env);
}

