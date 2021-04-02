#include <limo.h>
#include <openssl/ssl.h>

#define INS_SSL_BUILTIN(f, name) setq(limo_ssl_env, make_sym(name), make_builtin(f))
#define INS_SSL_VAR(val, name)   setq(limo_ssl_env, make_sym(name), val)

limo_data *sym_ssl;
limo_data *sym_sslctx;
limo_data *sym_sslconn;
limo_data *sym_sslmethod;

BUILTIN(builtin_ssl_tls_method)
{  return make_special(sym_sslmethod, TLS_method()); }
BUILTIN(builtin_ssl_tls_client_method)
{  return make_special(sym_sslmethod, TLS_client_method()); }
BUILTIN(builtin_ssl_tls_server_method)
{  return make_special(sym_sslmethod, TLS_server_method()); }

BUILTIN(builtin_ssl_new)
{
  limo_data *ld_ctx, *ld_res;
  SSL_CTX *ctx;
  REQUIRE_ARGC("SSL-NEW", 1);
  ld_ctx = eval(FIRST_ARG, env);
  ctx = get_special(ld_ctx, sym_sslctx);
  ld_res = make_special(sym_sslconn, SSL_new(ctx));
  // TODO: prevent contexts and other data structures from being collected if used
  // GC_register_finalizer()
  return ld_res;
}

BUILTIN(builtin_ssl_ctx_new)
{
  limo_data *ld_ssl_method, *ld_res;
  SSL_METHOD *ssl_method;
  SSL_CTX *ctx;

  REQUIRE_ARGC("SSL-CTX-NEW", 1);
  ld_ssl_method = eval(FIRST_ARG, env);
  ssl_method = get_special(ld_ssl_method, sym_sslmethod);
  ctx = SSL_CTX_new(ssl_method);
  if (!ctx) throw(make_cons(sym_ssl, make_string("couldn't create ssl-ctx")));
  ld_res = make_special(sym_sslctx, ctx);
  // TODO: prevent contexts and other data structures from being collected if used
  // GC_register_finalizer()
  return ld_res;
}

BUILTIN(builtin_ssl_set_fd)
{
  limo_data *ld_sslconn, *ld_fd;
  SSL *sslconn;
  int fd;
  
  REQUIRE_ARGC("SSL-SET-FD", 2);
  ld_sslconn = eval(FIRST_ARG, env);
  ld_fd  = eval(SECOND_ARG, env);
  REQUIRE_TYPE("SSL-SET-FD", ld_fd, limo_TYPE_GMPQ);
  sslconn = get_special(ld_sslconn, sym_sslconn);
  fd = GETINTFROMMPQ(ld_fd);
  if (!SSL_set_fd(sslconn, fd))
    throw(make_cons(sym_ssl, make_string("couldn't set fd to ssl-connection")));
  return nil;
}

BUILTIN(builtin_ssl_connect)
{
  limo_data *ld_sslconn;
  SSL *sslconn;
  REQUIRE_ARGC("SSL-CONNECT", 1);
  ld_sslconn = eval(FIRST_ARG, env);
  sslconn = get_special(ld_sslconn, sym_sslconn);
  if (1 != SSL_connect(sslconn))
    throw(make_cons(sym_ssl, make_string("couldn't connect")));
  return nil;
}

BUILTIN(builtin_ssl_accept)
{
  limo_data *ld_sslconn;
  SSL *sslconn;
  REQUIRE_ARGC("SSL-ACCEPT", 1);
  ld_sslconn = eval(FIRST_ARG, env);
  sslconn = get_special(ld_sslconn, sym_sslconn);
  if (1 != SSL_accept(sslconn))
    throw(make_cons(sym_ssl, make_string("couldn't accept")));
  return nil;
}

BUILTIN(builtin_ssl_ctx_use_certificate_file)
{
  limo_data *ld_ctx, *ld_filename, *ld_type;
  SSL_CTX *ctx;
  
  REQUIRE_ARGC("SSL-CTX-USE-CERTIFICATE-FILE", 3);
  ld_ctx = eval(FIRST_ARG, env);
  ld_filename = eval(SECOND_ARG, env);
  ld_type = eval(THIRD_ARG, env);

  ctx = get_special(ld_ctx, sym_sslctx);
  REQUIRE_TYPE("SSL-CTX-USE-CERTIFICATE-FILE", ld_filename, limo_TYPE_STRING);
  REQUIRE_TYPE("SSL-CTX-USE-CERTIFICATE-FILE", ld_type, limo_TYPE_GMPQ);
  if (1 != SSL_CTX_use_certificate_file(ctx, ld_filename->d_string, GETINTFROMMPQ(ld_type)))
    throw(make_cons(sym_ssl, make_string("couldn't assign certificate file")));
  return nil;
}

BUILTIN(builtin_ssl_use_privatekey_file)
{
  limo_data *ld_ctx, *ld_filename, *ld_type;
  SSL_CTX *ctx;
  
  REQUIRE_ARGC("SSL-CTX-USE-PRIVATEKEY-FILE", 3);
  ld_ctx = eval(FIRST_ARG, env);
  ld_filename = eval(SECOND_ARG, env);
  ld_type = eval(THIRD_ARG, env);

  ctx = get_special(ld_ctx, sym_sslctx);
  REQUIRE_TYPE("SSL-CTX-USE-PRIVATEKEY-FILE", ld_filename, limo_TYPE_STRING);
  REQUIRE_TYPE("SSL-CTX-USE-PRIVATEKEY-FILE", ld_type, limo_TYPE_GMPQ);
  if (1 != SSL_CTX_use_PrivateKey_file(ctx, ld_filename->d_string, GETINTFROMMPQ(ld_type)))
    throw(make_cons(sym_ssl, make_string("couldn't assign privatekey file")));
  return nil;
}

BUILTIN(builtin_ssl_read)
{
  limo_data *ld_sslconn, *ld_count, *ld_res;
  SSL *ssl;
  size_t count;
  ssize_t res;
  char *buf;

  REQUIRE_ARGC("SSL-READ", 2);
  ld_sslconn = eval(FIRST_ARG, env);
  ld_count = eval(SECOND_ARG, env);
  REQUIRE_TYPE("SSL-READ", ld_count, limo_TYPE_GMPQ);
  ssl = get_special(ld_sslconn, sym_sslconn);
  count = GETINTFROMMPQ(ld_count);
  buf = GC_malloc_atomic(count+1);
  res = SSL_read(ssl, buf, count);
  if (res < 0)
    throw(make_cons(sym_ssl, make_string("couldn't read from ssl-connection")));

  buf[res] = '\0';
  ld_res = make_limo_data();
  ld_res->type = limo_TYPE_STRING;
  ld_res->string_length = res;
  ld_res->d_string = buf;

  return ld_res;
}

BUILTIN(builtin_ssl_write)
{
  limo_data *ld_sslconn, *ld_buf;
  SSL *ssl;
  ssize_t res;

  REQUIRE_ARGC("SSL-WRITE", 2);
  ld_sslconn = eval(FIRST_ARG, env);
  ld_buf = eval(SECOND_ARG, env);
  ssl = get_special(ld_sslconn, sym_sslconn);
  REQUIRE_TYPE("SSL-WRITE", ld_buf, limo_TYPE_STRING);

  res = SSL_write(ssl,
		  ld_buf->d_string,
		  ld_buf->string_length);
  
  if (res < 0)
    throw(make_cons(sym_ssl, make_string("could't write to ssl-connection")));
  
  return make_number_from_long_long(res);
}

void limo_init_ssl(limo_data *env)
{
  limo_data *limo_ssl_env;
  sym_ssl = make_sym("SSL");
  sym_sslctx = make_sym("SSL-CTX");
  sym_sslconn = make_sym("SSL-CONNECTION");
  sym_sslmethod = make_sym("SSL-METHOD");

  SSL_load_error_strings();
  SSL_library_init();

  limo_ssl_env = make_env(nil);

  INS_SSL_BUILTIN(builtin_ssl_tls_method, "SSL-TLS-METHOD");
  INS_SSL_BUILTIN(builtin_ssl_tls_client_method, "SSL-TLS-CLIENT-METHOD");
  INS_SSL_BUILTIN(builtin_ssl_tls_server_method, "SSL-TLS-SERVER-METHOD");
  INS_SSL_BUILTIN(builtin_ssl_new, "SSL-NEW");
  INS_SSL_BUILTIN(builtin_ssl_ctx_new, "SSL-CTX-NEW");
  INS_SSL_BUILTIN(builtin_ssl_set_fd, "SSL-SET-FD");
  INS_SSL_BUILTIN(builtin_ssl_connect, "SSL-CONNECT");
  INS_SSL_BUILTIN(builtin_ssl_accept, "SSL-ACCEPT");
  INS_SSL_BUILTIN(builtin_ssl_read, "SSL-READ");
  INS_SSL_BUILTIN(builtin_ssl_write, "SSL-WRITE");
  INS_SSL_BUILTIN(builtin_ssl_ctx_use_certificate_file, "SSL-CTX-USE-CERTIFICATE-FILE");
  INS_SSL_BUILTIN(builtin_ssl_use_privatekey_file, "SSL-USE-PRIVATEKEY-FILE");

  INS_SSL_VAR(make_number_from_long_long(SSL_FILETYPE_PEM), "SSL_FILETYPE_PEM");
  
  setq(env, make_sym("_SSL"), limo_ssl_env);

  printf("TODO: rewrite ssl, this cannot check certificates yet!\n");
}

