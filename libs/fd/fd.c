#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifndef __MINGW32__
#include <poll.h>
#endif
#include <limo.h>

#define INS_FD_BUILTIN(f, name) setq(limo_fd_env, make_sym(name), make_builtin(f))
#define INS_FD_VAR(val, name)   setq(limo_fd_env, make_sym(name), val)

limo_data *sym_fd;

BUILTIN(builtin_fd_read)
{
  limo_data *ld_fd, *ld_count, *ld_res;
  size_t count;
  ssize_t res;
  char *buf;

  REQUIRE_ARGC("FD-READ", 2);
  ld_fd = eval(FIRST_ARG, env);
  ld_count = eval(SECOND_ARG, env);
  REQUIRE_TYPE("FD-READ", ld_fd, limo_TYPE_GMPQ);
  REQUIRE_TYPE("FD-READ", ld_count, limo_TYPE_GMPQ);
  count = GETINTFROMMPQ(ld_count);
  buf = GC_malloc_atomic(count+1);
  res = read(GETINTFROMMPQ(ld_fd), buf, count);
  if (res < 0)
    limo_error_errno(sym_fd);

  buf[res] = '\0';
  ld_res = make_limo_data();
  ld_res->type = limo_TYPE_STRING;
  ld_res->string_length = res;
  ld_res->d_string = buf;

  return ld_res;
}

BUILTIN(builtin_fd_write)
{
  limo_data *ld_fd, *ld_buf;
  ssize_t res;

  REQUIRE_ARGC("FD-WRITE", 2);
  ld_fd = eval(FIRST_ARG, env);
  ld_buf = eval(SECOND_ARG, env);
  REQUIRE_TYPE("FD-WRITE", ld_fd, limo_TYPE_GMPQ);
  REQUIRE_TYPE("FD-WRITE", ld_buf, limo_TYPE_STRING);

  res = write(GETINTFROMMPQ(ld_fd),
	      ld_buf->d_string,
	      ld_buf->string_length);
  
  if (res < 0) limo_error_errno(sym_fd);
  return make_number_from_long_long(res);
}

BUILTIN(builtin_fd_open)
{
  limo_data *ld_pathname, *ld_flags;
  int res;
  
  REQUIRE_ARGC("FD-OPEN", 2);
  ld_pathname = eval(FIRST_ARG, env);
  ld_flags = eval(SECOND_ARG, env);
  REQUIRE_TYPE("FD-OPEN", ld_pathname, limo_TYPE_STRING);
  REQUIRE_TYPE("FD-OPEN", ld_flags, limo_TYPE_GMPQ);
  res = open(ld_pathname->d_string, GETINTFROMMPQ(ld_flags));
  if (res < 0) limo_error_errno(sym_fd);
  return make_number_from_long_long(res);
}

BUILTIN(builtin_fd_close)
{
  limo_data *ld_fd;
  int fd;
  REQUIRE_ARGC("FD-CLOSE", 1);
  ld_fd = eval(FIRST_ARG, env);
  REQUIRE_TYPE("FD-CLOSE", ld_fd, limo_TYPE_GMPQ);
  fd = GETINTFROMMPQ(ld_fd);
  close(fd);

  return nil;
}

#ifndef __MINGW32__
BUILTIN(builtin_fd_poll)
{
  limo_data *ld_pollfdlist, *ld_timeout, *ld_res, *ld_cursor;
  int pollfdlist_length, i, timeout, res;
  struct pollfd *pfds;

  REQUIRE_ARGC("FD-POLL", 2);
  ld_pollfdlist = eval(FIRST_ARG, env);
  ld_timeout = eval(SECOND_ARG, env);
  REQUIRE_TYPE("FD-POLL", ld_pollfdlist, limo_TYPE_CONS);
  REQUIRE_TYPE("FD-POLL", ld_timeout, limo_TYPE_GMPQ);

  timeout = GETINTFROMMPQ(ld_timeout);

  pollfdlist_length = list_length(ld_pollfdlist);
  pfds = GC_malloc_atomic(pollfdlist_length * sizeof (struct pollfd));
  for (i=0; i<pollfdlist_length; ++i, ld_pollfdlist=CDR(ld_pollfdlist)) {
    REQUIRE_TYPE("FD-POLL", CAR(CAR(ld_pollfdlist)), limo_TYPE_GMPQ);
    REQUIRE_TYPE("FD-POLL", CDR(CAR(ld_pollfdlist)), limo_TYPE_GMPQ);
    pfds[i].fd = GETINTFROMMPQ(CAR(CAR(ld_pollfdlist)));
    pfds[i].events = GETINTFROMMPQ(CDR(CAR(ld_pollfdlist)));
    pfds[i].revents = 0;
  }

  res = poll(pfds, pollfdlist_length, timeout);
  if (0>res) limo_error_errno(sym_fd);

  ld_res = make_cons(nil, nil);
  ld_cursor = ld_res;
  
  for (i=0; i<pollfdlist_length; ++i,CDR(ld_cursor)=make_cons(nil, nil), ld_cursor=CDR(ld_cursor))
    CAR(ld_res) = make_number_from_long_long(pfds[i].revents);
  
  ld_cursor->type=limo_TYPE_NIL;
  return ld_res;
}
#endif

void limo_init_fd(limo_data *env)
{
  limo_data *limo_fd_env;
  sym_fd = make_sym("FD");

  limo_fd_env = make_env(nil);
  
  INS_FD_BUILTIN(builtin_fd_read, "FD-READ");
  INS_FD_BUILTIN(builtin_fd_write, "FD-WRITE");
  INS_FD_BUILTIN(builtin_fd_open, "FD-OPEN");
  INS_FD_BUILTIN(builtin_fd_close, "FD-CLOSE");
#ifndef __MINGW32__
  INS_FD_BUILTIN(builtin_fd_poll, "FD-POLL");
#endif

  INS_FD_VAR(make_number_from_long_long(O_APPEND), "O_APPEND");
  INS_FD_VAR(make_number_from_long_long(O_RDONLY), "O_RDONLY");
  INS_FD_VAR(make_number_from_long_long(O_WRONLY), "O_WRONLY");
  INS_FD_VAR(make_number_from_long_long(O_RDWR), "O_RDWR");
#ifndef __MINGW32__
  INS_FD_VAR(make_number_from_long_long(POLLIN), "POLLIN");
  INS_FD_VAR(make_number_from_long_long(POLLOUT), "POLLOUT");
  INS_FD_VAR(make_number_from_long_long(POLLERR), "POLLERR");
  INS_FD_VAR(make_number_from_long_long(POLLHUP), "POLLHUP");
#endif
  setq(env, make_sym("_FD"), limo_fd_env);
}
