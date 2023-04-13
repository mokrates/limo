#include <limo.h>
#include <openssl/ssl.h>

#define INS_SSL_BUILTIN(f, name)    setq(limo_ssl_env, make_sym(name), make_builtin(f, name))
#define INS_SSL_BUILTINFUN(f, name) setq(limo_ssl_env, make_sym(name), make_builtinfun(f, name))
#define INS_SSL_VAR(val, name)      setq(limo_ssl_env, make_sym(name), val)

limo_data *sym_ssl;
limo_data *sym_sslctx;
limo_data *sym_sslconn;
limo_data *sym_sslmethod;

BUILTIN(builtin_ssl_tls_method)
{  return make_special(sym_sslmethod, TLS_method()); }
BUILTIN(builtin_ssl_tls_client_method)
{  return make_special(sym_sslmethod, TLS_client_method()); }

/////// this describes, how a ssl-server should be configured
/// TODO
/// https://wiki.openssl.org/index.php/Simple_TLS_Server

//// TODO ssl.limo-wrapper.

static ssl_error_to_string(const SSL *ssl, int ret)
{
  switch (SSL_get_error(ssl, ret)) {
  case SSL_ERROR_NONE: return make_string("SSL_ERROR_NONE");
  case SSL_ERROR_ZERO_RETURN: return make_string("SSL_ERROR_ZERO_RETURN");
  case SSL_ERROR_WANT_READ: return make_string("SSL_ERROR_WANT_READ");
  case SSL_ERROR_WANT_WRITE: return make_string("SSL_ERROR_WANT_WRITE");
  case SSL_ERROR_WANT_CONNECT: return make_string("SSL_ERROR_WANT_CONNECT");
  case SSL_ERROR_WANT_ACCEPT: return make_string("SSL_ERROR_WANT_ACCEPT");
  case SSL_ERROR_WANT_X509_LOOKUP: return make_string("SSL_ERROR_WANT_X509_LOOKUP");
  case SSL_ERROR_WANT_ASYNC: return make_string("SSL_ERROR_WANT_ASYNC");
  case SSL_ERROR_WANT_ASYNC_JOB: return make_string("SSL_ERROR_WANT_ASYNC_JOB");
  case SSL_ERROR_WANT_CLIENT_HELLO_CB: return make_string("SSL_ERROR_WANT_CLIENT_HELLO_CB");
  case SSL_ERROR_SYSCALL: return make_string("SSL_ERROR_SYSCALL");
  case SSL_ERROR_SSL: return make_string("SSL_ERROR_SSL");
  }
}


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
  return ld_res;   // user has to retain a reference.
}

BUILTINFUN(builtin_SSL_CTX_set_default_verify_paths)
{
  SSL_CTX *ctx;
  
  REQUIRE_ARGC_FUN("SSL-CTX-SET-DEFAULT-VERIFY-PATHS", 1);
  ctx = get_special(argv[0], sym_sslctx);

  if (SSL_CTX_set_default_verify_paths(ctx) != 1)
    throw(make_cons(sym_ssl, make_string("Error loading trust store")));
    
  return nil;
}

BUILTINFUN(builtin_SSL_CTX_set_verify_simple)
{
  SSL_CTX *ctx;
  int mode;
  
  REQUIRE_ARGC_FUN("SSL_CTX_set_verify", 2);
  ctx = get_special(argv[0], sym_sslctx);
  REQUIRE_TYPE("SSL_CTX_set_verify", argv[1], limo_TYPE_GMPQ);
  mode = GETINTFROMMPQ(argv[1]);

  SSL_CTX_set_verify(ctx, mode, NULL); // no callback, hence '_simple'

  return nil;
}

BUILTINFUN(builtin_SSL_set1_host)
{
  SSL *sslconn;
  
  REQUIRE_ARGC_FUN("SSL_set1_host", 2);
  sslconn = get_special(argv[0], sym_sslconn);
  REQUIRE_TYPE("SSL_set1_host", argv[1], limo_TYPE_STRING);
  if (!SSL_set1_host(sslconn, argv[1]->d_string))
    throw(make_cons(sym_ssl, make_string("Couldn't set hostname")));

  return nil;
}

BUILTINFUN(builtin_SSL_set_tlsext_host_name)
{
  SSL *sslconn;
  
  REQUIRE_ARGC_FUN("SSL_set_tlsext_host_name", 2);
  sslconn = get_special(argv[0], sym_sslconn);
  REQUIRE_TYPE("SSL_set_tlsext_host_name", argv[1], limo_TYPE_STRING);
  if (!SSL_set_tlsext_host_name(sslconn, argv[1]->d_string))
    throw(make_cons(sym_ssl, make_string("Couldn't set SNI hostname")));

  return nil;
}

BUILTINFUN(builtin_SSL_get_verify_result)
{
  SSL *sslconn;
  long res;
  REQUIRE_ARGC_FUN("SSL_get_verify_result", 1);
  sslconn = get_special(argv[0], sym_sslconn);

  res = SSL_get_verify_result(sslconn);
  switch (res) {
  case X509_V_OK: return make_string("X509_V_OK");
  case X509_V_ERR_UNSPECIFIED: return make_string("X509_V_ERR_UNSPECIFIED");
  case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: return make_string("X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT");
  case X509_V_ERR_UNABLE_TO_GET_CRL: return make_string("X509_V_ERR_UNABLE_TO_GET_CRL");
  case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE: return make_string("X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE");
  case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE: return make_string("X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE");
  case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY: return make_string("X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY");
  case X509_V_ERR_CERT_SIGNATURE_FAILURE: return make_string("X509_V_ERR_CERT_SIGNATURE_FAILURE");
  case X509_V_ERR_CRL_SIGNATURE_FAILURE: return make_string("X509_V_ERR_CRL_SIGNATURE_FAILURE");
  case X509_V_ERR_CERT_NOT_YET_VALID: return make_string("X509_V_ERR_CERT_NOT_YET_VALID");
  case X509_V_ERR_CERT_HAS_EXPIRED: return make_string("X509_V_ERR_CERT_HAS_EXPIRED");
  case X509_V_ERR_CRL_NOT_YET_VALID: return make_string("X509_V_ERR_CRL_NOT_YET_VALID");
  case X509_V_ERR_CRL_HAS_EXPIRED: return make_string("X509_V_ERR_CRL_HAS_EXPIRED");
  case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD: return make_string("X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD");
  case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD: return make_string("X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD");
  case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD: return make_string("X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD");
  case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD: return make_string("X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD");
  case X509_V_ERR_OUT_OF_MEM: return make_string("X509_V_ERR_OUT_OF_MEM");
  case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT: return make_string("X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT");
  case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN: return make_string("X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN");
  case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY: return make_string("X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY");
  case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE: return make_string("X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE");
  case X509_V_ERR_CERT_CHAIN_TOO_LONG: return make_string("X509_V_ERR_CERT_CHAIN_TOO_LONG");
  case X509_V_ERR_CERT_REVOKED: return make_string("X509_V_ERR_CERT_REVOKED");
  case X509_V_ERR_INVALID_CA: return make_string("X509_V_ERR_INVALID_CA");
  case X509_V_ERR_PATH_LENGTH_EXCEEDED: return make_string("X509_V_ERR_PATH_LENGTH_EXCEEDED");
  case X509_V_ERR_INVALID_PURPOSE: return make_string("X509_V_ERR_INVALID_PURPOSE");
  case X509_V_ERR_CERT_UNTRUSTED: return make_string("X509_V_ERR_CERT_UNTRUSTED");
  case X509_V_ERR_CERT_REJECTED: return make_string("X509_V_ERR_CERT_REJECTED");
  case X509_V_ERR_SUBJECT_ISSUER_MISMATCH: return make_string("X509_V_ERR_SUBJECT_ISSUER_MISMATCH");
  case X509_V_ERR_AKID_SKID_MISMATCH: return make_string("X509_V_ERR_AKID_SKID_MISMATCH");
  case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH: return make_string("X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH");
  case X509_V_ERR_KEYUSAGE_NO_CERTSIGN: return make_string("X509_V_ERR_KEYUSAGE_NO_CERTSIGN");
  case X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER: return make_string("X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER");
  case X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION: return make_string("X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION");
  case X509_V_ERR_KEYUSAGE_NO_CRL_SIGN: return make_string("X509_V_ERR_KEYUSAGE_NO_CRL_SIGN");
  case X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION: return make_string("X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION");
  case X509_V_ERR_INVALID_NON_CA: return make_string("X509_V_ERR_INVALID_NON_CA");
  case X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED: return make_string("X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED");
    //  case X509_V_ERR_PROXY_SUBJECT_INVALID: return make_string("X509_V_ERR_PROXY_SUBJECT_INVALID");
  case X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE: return make_string("X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE");
  case X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED: return make_string("X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED");
  case X509_V_ERR_INVALID_EXTENSION: return make_string("X509_V_ERR_INVALID_EXTENSION");
  case X509_V_ERR_INVALID_POLICY_EXTENSION: return make_string("X509_V_ERR_INVALID_POLICY_EXTENSION");
  case X509_V_ERR_NO_EXPLICIT_POLICY: return make_string("X509_V_ERR_NO_EXPLICIT_POLICY");
  case X509_V_ERR_DIFFERENT_CRL_SCOPE: return make_string("X509_V_ERR_DIFFERENT_CRL_SCOPE");
  case X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE: return make_string("X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE");
  case X509_V_ERR_UNNESTED_RESOURCE: return make_string("X509_V_ERR_UNNESTED_RESOURCE");
  case X509_V_ERR_PERMITTED_VIOLATION: return make_string("X509_V_ERR_PERMITTED_VIOLATION");
  case X509_V_ERR_EXCLUDED_VIOLATION: return make_string("X509_V_ERR_EXCLUDED_VIOLATION");
  case X509_V_ERR_SUBTREE_MINMAX: return make_string("X509_V_ERR_SUBTREE_MINMAX");
  case X509_V_ERR_APPLICATION_VERIFICATION: return make_string("X509_V_ERR_APPLICATION_VERIFICATION");
  case X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE: return make_string("X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE");
  case X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX: return make_string("X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX");
  case X509_V_ERR_UNSUPPORTED_NAME_SYNTAX: return make_string("X509_V_ERR_UNSUPPORTED_NAME_SYNTAX");
  case X509_V_ERR_CRL_PATH_VALIDATION_ERROR: return make_string("X509_V_ERR_CRL_PATH_VALIDATION_ERROR");
  case X509_V_ERR_PATH_LOOP: return make_string("X509_V_ERR_PATH_LOOP");
  case X509_V_ERR_SUITE_B_INVALID_VERSION: return make_string("X509_V_ERR_SUITE_B_INVALID_VERSION");
  case X509_V_ERR_SUITE_B_INVALID_ALGORITHM: return make_string("X509_V_ERR_SUITE_B_INVALID_ALGORITHM");
  case X509_V_ERR_SUITE_B_INVALID_CURVE: return make_string("X509_V_ERR_SUITE_B_INVALID_CURVE");
  case X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM: return make_string("X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM");
  case X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED: return make_string("X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED");
  case X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256: return make_string("X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256");
  case X509_V_ERR_HOSTNAME_MISMATCH: return make_string("X509_V_ERR_HOSTNAME_MISMATCH");
  case X509_V_ERR_EMAIL_MISMATCH: return make_string("X509_V_ERR_EMAIL_MISMATCH");
  case X509_V_ERR_IP_ADDRESS_MISMATCH: return make_string("X509_V_ERR_IP_ADDRESS_MISMATCH");
  case X509_V_ERR_DANE_NO_MATCH: return make_string("X509_V_ERR_DANE_NO_MATCH");
  case X509_V_ERR_EE_KEY_TOO_SMALL: return make_string("X509_V_ERR_EE_KEY_TOO_SMALL");
    // case X509_ERR_CA_KEY_TOO_SMALL: return make_string("X509_ERR_CA_KEY_TOO_SMALL");
    // case X509_ERR_CA_MD_TOO_WEAK: return make_string("X509_ERR_CA_MD_TOO_WEAK");
  case X509_V_ERR_INVALID_CALL: return make_string("X509_V_ERR_INVALID_CALL");
  case X509_V_ERR_STORE_LOOKUP: return make_string("X509_V_ERR_STORE_LOOKUP");
  case X509_V_ERR_NO_VALID_SCTS: return make_string("X509_V_ERR_NO_VALID_SCTS");
  case X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION: return make_string("X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION");
  case X509_V_ERR_OCSP_VERIFY_NEEDED: return make_string("X509_V_ERR_OCSP_VERIFY_NEEDED");
  case X509_V_ERR_OCSP_VERIFY_FAILED: return make_string("X509_V_ERR_OCSP_VERIFY_FAILED");
  case X509_V_ERR_OCSP_CERT_UNKNOWN: return make_string("X509_V_ERR_OCSP_CERT_UNKNOWN");
  default: return make_string("LIMO_SSL_UNKNOWN_ERROR");
  }
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
  int ret;
  REQUIRE_ARGC("SSL-CONNECT", 1);
  ld_sslconn = eval(FIRST_ARG, env);
  sslconn = get_special(ld_sslconn, sym_sslconn);
  if (1 != SSL_connect(sslconn))
    throw(make_cons(sym_ssl, ssl_error_to_string(sslconn, ret)));
  return nil;
}

BUILTIN(builtin_ssl_accept)
{
  limo_data *ld_sslconn;
  SSL *sslconn;
  int ret;
  REQUIRE_ARGC("SSL-ACCEPT", 1);
  ld_sslconn = eval(FIRST_ARG, env);
  sslconn = get_special(ld_sslconn, sym_sslconn);
  if (1 != (ret = SSL_accept(sslconn)))
    throw(make_cons(sym_ssl, ssl_error_to_string(sslconn, ret)));
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

BUILTIN(builtin_ssl_ctx_use_privatekey_file)
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
  
  return make_rational_from_long_long(res);
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
  INS_SSL_BUILTINFUN(builtin_SSL_CTX_set_default_verify_paths, "SSL-CTX-SET-DEFAULT-VERIFY-PATHS");
  INS_SSL_BUILTINFUN(builtin_SSL_CTX_set_verify_simple, "SSL-CTX-SET-VERIFY-SIMPLE");
  INS_SSL_BUILTINFUN(builtin_SSL_set1_host, "SSL-SET1-HOST");
  INS_SSL_BUILTINFUN(builtin_SSL_set_tlsext_host_name, "SSL-SET-TLSEXT-HOST-NAME");
  INS_SSL_BUILTINFUN(builtin_SSL_get_verify_result, "SSL-GET-VERIFY-RESULT");
  INS_SSL_BUILTIN(builtin_ssl_set_fd, "SSL-SET-FD");
  INS_SSL_BUILTIN(builtin_ssl_connect, "SSL-CONNECT");
  INS_SSL_BUILTIN(builtin_ssl_accept, "SSL-ACCEPT");
  INS_SSL_BUILTIN(builtin_ssl_read, "SSL-READ");
  INS_SSL_BUILTIN(builtin_ssl_write, "SSL-WRITE");
  INS_SSL_BUILTIN(builtin_ssl_ctx_use_certificate_file, "SSL-CTX-USE-CERTIFICATE-FILE");
  INS_SSL_BUILTIN(builtin_ssl_ctx_use_privatekey_file, "SSL-CTX-USE-PRIVATEKEY-FILE");

  INS_SSL_VAR(make_rational_from_long_long(SSL_FILETYPE_PEM), "SSL_FILETYPE_PEM");
  
  setq(env, make_sym("_SSL"), limo_ssl_env);

  printf("TODO: rewrite ssl, this cannot check certificates yet!\n");
}

