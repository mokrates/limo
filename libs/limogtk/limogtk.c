#include "limogtk.h"

limo_data *sym_limogtk_gpointer;

#define INS_LIMOGTK_BUILTIN(f, name) setq(limo_limogtk_env, make_sym(name), make_builtin(f, name))
#define INS_LIMOGTK_BUILTINFUN(f, name) setq(limo_limogtk_env, make_sym(name), make_builtinfun(f, name))
#define INS_LIMOGTK_FLAG(name, val)  setq(limo_limogtk_env, make_sym(name), make_rational_from_long_long(val));

static void limogtk_finalizer_g_object_unref(limo_data *ld, void *user_data)
{
  g_object_unref(user_data);
}

BUILTIN(builtin_gtk_init)
{
  gtk_init(NULL, NULL);
  return nil;
}

BUILTIN(builtin_gtk_init_check)
{
  return make_rational_from_long_long(gtk_init_check(NULL, NULL));
}

BUILTIN(builtin_g_application_run)
{
  limo_data *ld_app;
  REQUIRE_ARGC("G-APPLICATION-RUN", 1);
  ld_app = eval(FIRST_ARG, env);

  return make_rational_from_long_long(g_application_run(G_APPLICATION(get_special(ld_app, sym_limogtk_gpointer)), 0, NULL));
}

static void *limogtk_signal_handler(GtkWidget *widget, gpointer *user_data)
{
  limo_data *ld_handler_call, *result;
  ld_handler_call = make_cons(CAR((limo_data *)user_data), make_cons(make_special(sym_limogtk_gpointer, widget), nil));
  result = eval(ld_handler_call, CDR((limo_data *)user_data));
  return result;
}

BUILTIN(builtin_g_signal_connect_simple)
{
  limo_data *ld_instance, *ld_signal, *ld_handler;

  REQUIRE_ARGC("G-SIGNAL-CONNECT", 3);
  ld_instance = eval(FIRST_ARG, env);
  ld_signal   = eval(SECOND_ARG, env);
  ld_handler  = eval(THIRD_ARG, env);
  REQUIRE_TYPE("G-SIGNAL-CONNECT-SIMPLE", ld_signal, limo_TYPE_STRING);
  REQUIRE_TYPE("G-SIGNAL-CONNECT-SIMPLE", ld_handler, limo_TYPE_LAMBDA);
  REQUIRE_TYPE("G-SIGNAL-CONNECT-SIMPLE", ld_instance, limo_TYPE_SPECIAL);
  return make_rational_from_long_long(g_signal_connect(G_OBJECT(ld_instance->d_special_intern),
                                                     ld_signal->d_string,
                                                     G_CALLBACK(limogtk_signal_handler),
                                                     (gpointer)make_cons(ld_handler, env)));
}

BUILTIN(builtin_make_gtk_text_iter)
{
  gpointer iter;
  iter = GC_malloc_atomic(sizeof (GtkTextIter));
  return make_special(sym_limogtk_gpointer, iter);
}

BUILTIN(builtin_make_errorpp)
{
  GError **gerrpp;
  gerrpp = GC_malloc_atomic(sizeof (GError *));
  *gerrpp = NULL;
  return make_special(sym_limogtk_gpointer, gerrpp);
}

BUILTIN(builtin_get_error)
{
  limo_data *ld_gerrpp;
  ld_gerrpp = eval(FIRST_ARG, env);
  GError **gerrpp = get_special(ld_gerrpp, sym_limogtk_gpointer);
  if (*gerrpp == NULL)
    return nil;
  else
    return make_string((*gerrpp)->message);
}

#include "_gtk-functions.h"
#include "gtk-marshall.h"

static gboolean limo_g_timeout_cb(limo_data *closure)
{
  limo_data *res = eval(CAR(closure), CDR(closure));
  return !is_nil(res);
}

BUILTIN(builtin_g_timeout_add)
{
  limo_data *ld_interval;  // in msecs
  limo_data *ld_fun;
  limo_data *closure;
  int event_source;

  REQUIRE_ARGC("G-TIMEOUT-ADD", 2);
  ld_interval = eval(FIRST_ARG, env);
  ld_fun = eval(SECOND_ARG, env);
  REQUIRE_TYPE("G-TIMEOUT-ADD", ld_interval, limo_TYPE_GMPQ);
  REQUIRE_TYPE("G-TIMEOUT-ADD", ld_fun, limo_TYPE_LAMBDA);

  closure = make_cons(make_cons(ld_fun, nil), env);
  event_source = g_timeout_add(GETINTFROMMPQ(ld_interval), (GSourceFunc)limo_g_timeout_cb, closure);
  return make_cons(make_rational_from_long_long(event_source), closure);   // should be saved by caller to prevent collection
}

BUILTIN(builtin_g_timeout_add_seconds)
{
  limo_data *ld_interval;  // in msecs
  limo_data *ld_fun;
  limo_data *closure;
  int event_source;

  REQUIRE_ARGC("G-TIMEOUT-ADD-SECONDS", 2);
  ld_interval = eval(FIRST_ARG, env);
  ld_fun = eval(SECOND_ARG, env);
  REQUIRE_TYPE("G-TIMEOUT-ADD-SECONDS", ld_interval, limo_TYPE_GMPQ);
  REQUIRE_TYPE("G-TIMEOUT-ADD-SECONDS", ld_fun, limo_TYPE_LAMBDA);

  closure = make_cons(make_cons(ld_fun, nil), env);
  event_source = g_timeout_add_seconds(GETINTFROMMPQ(ld_interval), (GSourceFunc)limo_g_timeout_cb, closure);
  return make_cons(make_rational_from_long_long(event_source), closure);   // should be saved by caller to prevent collection
}

void limo_init_limogtk(limo_data *env)
{
  limo_data *limo_limogtk_env;
  sym_limogtk_gpointer = make_sym("LIMOGTK-GPOINTER");
  limo_limogtk_env = make_env(nil);

  INS_LIMOGTK_BUILTIN(builtin_gtk_init, "GTK-INIT");
  INS_LIMOGTK_BUILTIN(builtin_gtk_init_check, "GTK-INIT-CHECK");
  INS_LIMOGTK_BUILTIN(builtin_g_application_run, "G-APPLICATION-RUN");
  INS_LIMOGTK_BUILTIN(builtin_g_signal_connect_simple, "G-SIGNAL-CONNECT-SIMPLE");
  INS_LIMOGTK_BUILTIN(builtin_g_signal_connect, "G-SIGNAL-CONNECT");
  INS_LIMOGTK_BUILTIN(builtin_g_timeout_add, "G-TIMEOUT-ADD");
  INS_LIMOGTK_BUILTIN(builtin_g_timeout_add_seconds, "G-TIMEOUT-ADD-SECONDS");
  INS_LIMOGTK_BUILTIN(builtin_make_gtk_text_iter, "MAKE-GTK-TEXT-ITER");
  INS_LIMOGTK_BUILTIN(builtin_make_errorpp, "MAKE-ERRORPP");
  INS_LIMOGTK_BUILTIN(builtin_get_error, "GET-ERROR");
  
  #include "_gtk-inserts.h"
  #include "_limogtk-enums.h"

  setq(env, make_sym("_GTK"), limo_limogtk_env);
}
