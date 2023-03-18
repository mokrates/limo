#include <limo.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define __USE_GNU
#include <unistd.h>
#include <fcntl.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f, name))

//////////////////////////////////////////////////////////////////////////////
/// stolen from
/// https://gist.github.com/mike-bourgeous/2be6c8900bf624887fe5fee4f28552ef
/// but then adapted
//////////////////////////////////////////////////////////////////////////////
/* Mike Bourgeous
 * https://github.com/nitrogenlogic
 */


/*
 * Sets the FD_CLOEXEC flag.  Returns 0 on success, -1 on error.
 */
static int set_cloexec(int fd)
{
	if(fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) == -1) {
		perror("Error setting FD_CLOEXEC flag");
		return -1;
	}

	return 0;
}

/*
 * Runs command in another process, with full remote interaction capabilities.
 * Writing to *writefd will write to the command's stdin.  Reading from *readfd
 * will read from the command's stdout.  Reading from *errfd will read from the
 * command's stderr.  If NULL is passed for writefd, readfd, or errfd, then the
 * command's stdin, stdout, or stderr will not be changed.  Returns the child
 * PID on success, -1 on error.
 */
static pid_t popen3(char *command, char *const argv[], char *const envp[],
             int *writefd, int *readfd, int *errfd)
{
  int in_pipe[2]  = {-1, -1};
  int out_pipe[2] = {-1, -1};
  int err_pipe[2] = {-1, -1};
  pid_t pid;
  char *error_msg;

  // 2011 implementation of popen3() by Mike Bourgeous
  // https://gist.github.com/1022231  

  if(command == NULL) {
    limo_error("Cannot popen3() a NULL command.");
  }

  if(writefd && pipe(in_pipe)) {
    error_msg = "Error creating pipe for stdin";
    goto error;
  }
  if(readfd && pipe(out_pipe)) {
    error_msg = "Error creating pipe for stdout";
    goto error;
  }
  if(errfd && pipe(err_pipe)) {
    error_msg = "Error creating pipe for stderr";
    goto error;
  }

  pid = fork();
  switch(pid) {
  case -1:
    // Error
    error_msg = "Error creating child process";
    goto error;
  case 0:
    // Child
    if(writefd) {
      close(in_pipe[1]);
      if(dup2(in_pipe[0], 0) == -1) {
        perror("Error assigning stdin in child process");
        exit(-1);
      }
      close(in_pipe[0]);
    }
    if(readfd) {
      close(out_pipe[0]);
      if(dup2(out_pipe[1], 1) == -1) {
        perror("Error assigning stdout in child process");
        exit(-1);
      }
      close(out_pipe[1]);
    }
    if(errfd) {
      close(err_pipe[0]);
      if(dup2(err_pipe[1], 2) == -1) {
        perror("Error assigning stderr in child process");
        exit(-1);
      }
      close(err_pipe[1]);
    }
    
    if (argv && envp)
      execvpe(command, argv, envp);
    else if (argv)
      execvp(command, argv);
    else
      execlp(command, command, (char *)NULL);

    perror("Error executing command in child process");
    exit(-1);

  default:
    // Parent
    break;
  }

  if(writefd) {
    close(in_pipe[0]);
    set_cloexec(in_pipe[1]);
    *writefd = in_pipe[1];
  }
  if(readfd) {
    close(out_pipe[1]);
    set_cloexec(out_pipe[0]);
    *readfd = out_pipe[0];
  }
  if(errfd) {
    close(err_pipe[1]);
    set_cloexec(out_pipe[0]);
    *errfd = err_pipe[0];
  }

  return pid;

 error:
  if(in_pipe[0] >= 0) {
    close(in_pipe[0]);
  }
  if(in_pipe[1] >= 0) {
    close(in_pipe[1]);
  }
  if(out_pipe[0] >= 0) {
    close(out_pipe[0]);
  }
  if(out_pipe[1] >= 0) {
    close(out_pipe[1]);
  }
  if(err_pipe[0] >= 0) {
    close(err_pipe[0]);
  }
  if(err_pipe[1] >= 0) {
    close(err_pipe[1]);
  }

  return -1;
}


BUILTIN(builtin_popen)
{
  limo_data *ld_filename, *ld_argv, *ld_env, *ld_wstdin, *ld_wstdout, *ld_wstderr;
  int out_stdin=-1, out_stdout=-1, out_stderr=-1;
  char **ca_argv=NULL, **ca_env=NULL;
  int ret;

  REQUIRE_ARGC("POPEN", 6);

  ld_filename = eval(FIRST_ARG, env);
  ld_argv     = eval(SECOND_ARG, env);
  ld_env      = eval(THIRD_ARG, env);
  ld_wstdin   = eval(FOURTH_ARG, env);
  ld_wstdout  = eval(FIFTH_ARG, env);
  ld_wstderr  = eval(SIXTH_ARG, env);

  REQUIRE_TYPE("POPEN", ld_filename, limo_TYPE_STRING);

  if (!is_nil(ld_argv)) {
    int i, l;
    l = list_length(ld_argv);
    ca_argv = GC_malloc((l+1) * sizeof (char *));
    for (i=0; i<l; ++i, ld_argv=CDR(ld_argv))
      ca_argv[i]=CAR(ld_argv)->d_string;
    ca_argv[i] = NULL;
  }
  if (!is_nil(ld_env)) {
    int i, l;
    l = list_length(ld_env);
    ca_env = GC_malloc((l+1) * sizeof (char *));
    for (i=0; i<l; ++i, ld_env=CDR(ld_env))
      ca_env[i]=CAR(ld_env)->d_string;
    ca_env[i] = NULL;
  }

  ret = popen3(ld_filename->d_string,
               ca_argv,
               ca_env,
               is_nil(ld_wstdin)?NULL:&out_stdin,
               is_nil(ld_wstdout)?NULL:&out_stdout,
               is_nil(ld_wstderr)?NULL:&out_stderr);
  return make_cons(make_cons(make_sym("PID"), make_rational_from_long_long(ret)),
                   make_cons(make_cons(make_sym("STDIN"),
                                       make_rational_from_long_long(out_stdin)),
                             make_cons(make_cons(make_sym("STDOUT"),
                                                 make_rational_from_long_long(out_stdout)),
                                       make_cons(make_cons(make_sym("STDERR"),
                                                           make_rational_from_long_long(out_stderr)),
                                                 make_nil()))));
}

BUILTIN(builtin_popen_wait)
{
  limo_data *ld_pid;
  pid_t pid, res;
  int stat_loc;
  char buf[512];

  REQUIRE_ARGC("POPEN-WAIT", 1);
  ld_pid = eval(FIRST_ARG, env);
  REQUIRE_TYPE("POPEN-WAIT", ld_pid, limo_TYPE_GMPQ);
  pid = GETINTFROMMPQ(ld_pid);

  res = waitpid(pid, &stat_loc, 0);
  if (-1 == res) {
    strerror_r(errno, buf, 512);
    throw(make_cons(make_sym("WAIT"), make_cons(make_string(buf), nil)));
  }
  return make_rational_from_long_long(WEXITSTATUS(stat_loc));
}

void limo_init_popen(limo_data *env)
{
  INSBUILTIN(builtin_popen, "popen");
  INSBUILTIN(builtin_popen_wait, "popen-wait");
}
