#include "limogtk.h"

limo_data *sym_limogtk_gpointer;

#define INS_LIMOGTK_BUILTIN(f, name) setq(limo_limogtk_env, make_sym(name), make_builtin(f))
#define INS_LIMOGTK_FLAG(name, val)  setq(limo_limogtk_env, make_sym(name), make_number_from_long_long(val));

static void limogtk_finalizer_g_object_unref(limo_data *ld, void *user_data)
{
  g_object_unref(user_data);
}

BUILTIN(builtin_gtk_application_window_new)
{
  limo_data *ld_app;
  gpointer app;
  REQUIRE_ARGC("GTK-APPLICATION-WINDOW-NEW", 1);
  ld_app = eval(FIRST_ARG, env);
  app = get_special(ld_app, sym_limogtk_gpointer);
  return make_special(sym_limogtk_gpointer, gtk_application_window_new(GTK_APPLICATION(app)));
}

BUILTIN(builtin_gtk_window_set_title)
{
  limo_data *ld_window, *ld_title;
  GtkWidget *window;
  REQUIRE_ARGC("GTK-WINDOW-SET-TITLE", 2);
  ld_window = eval(FIRST_ARG, env);
  ld_title = eval(SECOND_ARG, env);
  window = get_special(ld_window, sym_limogtk_gpointer);
  REQUIRE_TYPE("GTK-WINDOW-SET-TITLE", ld_title, limo_TYPE_STRING);
  gtk_window_set_title(GTK_WINDOW(window), ld_title->data.d_string);
  return nil;
}

BUILTIN(builtin_gtk_window_set_default_size)
{
  limo_data *ld_window, *ld_width, *ld_height;
  GtkWidget *window;
  REQUIRE_ARGC("GTK-WINDOW-SET-DEFAULT-SIZE", 3);
  ld_window = eval(FIRST_ARG, env);
  ld_width = eval(SECOND_ARG, env);
  ld_height = eval(THIRD_ARG, env);
  window = get_special(ld_window, sym_limogtk_gpointer);
  REQUIRE_TYPE("GTK-WINDOW-SET-DEFAULT-SIZE", ld_width, limo_TYPE_GMPQ);
  REQUIRE_TYPE("GTK-WINDOW-SET-DEFAULT-SIZE", ld_height, limo_TYPE_GMPQ);
  gtk_window_set_default_size(GTK_WINDOW(window), GETINTFROMMPQ(ld_width), GETINTFROMMPQ(ld_height));
  return nil;
}

BUILTIN(builtin_gtk_widget_show_all)
{
  limo_data *ld_widget;
  GtkWidget *widget;
  REQUIRE_ARGC("GTK-WIDGET-SHOW-ALL", 1);
  ld_widget = eval(FIRST_ARG, env);
  widget = get_special(ld_widget, sym_limogtk_gpointer);
  gtk_widget_show_all(GTK_WIDGET(widget));
  return nil;
}

BUILTIN(builtin_g_application_run)
{
  limo_data *ld_app;
  REQUIRE_ARGC("G-APPLICATION-RUN", 1);
  ld_app = eval(FIRST_ARG, env);

  return make_number_from_long_long(g_application_run(G_APPLICATION(get_special(ld_app, sym_limogtk_gpointer)), 0, NULL));
}

static void *limogtk_signal_handler(GtkWidget *widget, gpointer *user_data)
{
  limo_data *ld_handler_call, *result;
  ld_handler_call = make_cons(CAR((limo_data *)user_data), make_cons(make_special(sym_limogtk_gpointer, widget), nil));
  result = eval(ld_handler_call, CDR((limo_data *)user_data));
  return result;
}

BUILTIN(builtin_g_signal_connect)
{
  limo_data *ld_instance, *ld_signal, *ld_handler;

  REQUIRE_ARGC("G-SIGNAL-CONNECT", 3);
  ld_instance = eval(FIRST_ARG, env);
  ld_signal   = eval(SECOND_ARG, env);
  ld_handler  = eval(THIRD_ARG, env);
  REQUIRE_TYPE("G-SIGNAL-CONNECT", ld_signal, limo_TYPE_STRING);
  REQUIRE_TYPE("G-SIGNAL-CONNECT", ld_handler, limo_TYPE_LAMBDA);
  REQUIRE_TYPE("G-SIGNAL-CONNECT", ld_instance, limo_TYPE_SPECIAL);
  return make_number_from_long_long(g_signal_connect(G_OBJECT(CDR(ld_instance->data.d_special) -> data.d_special_intern),
                                                     ld_signal->data.d_string,
                                                     G_CALLBACK(limogtk_signal_handler),
                                                     (gpointer)make_cons(ld_handler, env)));
}

BUILTIN(builtin_gtk_application_new)
{
  limo_data *ld_app_id, *ld_app_flags, *ld_result;
  GtkApplication *app;
  char *app_id;
  GApplicationFlags flags;

  REQUIRE_ARGC("GTK-APPLICATION-NEW", 2);
  ld_app_id = eval(FIRST_ARG, env);
  ld_app_flags = eval(SECOND_ARG, env);
  if (is_nil(ld_app_id))
    app_id = NULL;
  else {
    REQUIRE_TYPE("GTK-APPLICATION-NEW", ld_app_id, limo_TYPE_STRING);
    app_id = ld_app_id->data.d_string;
  }
  REQUIRE_TYPE("GTK-APPLICATION-NEW", ld_app_flags, limo_TYPE_GMPQ);
  flags  = GETINTFROMMPQ(ld_app_flags);
  app = gtk_application_new(app_id, flags);
  ld_result =  make_special(sym_limogtk_gpointer, app);
  GC_register_finalizer(ld_result, (GC_finalization_proc)limogtk_finalizer_g_object_unref, app, NULL, NULL);
  return ld_result;
}

#include "gtk-functions.h"

void limo_init_limogtk(limo_data *env)
{
  limo_data *limo_limogtk_env;
  sym_limogtk_gpointer = make_sym("LIMOGTK-GPOINTER");
  limo_limogtk_env = make_env(nil);

  INS_LIMOGTK_BUILTIN(builtin_g_application_run, "G-APPLICATION-RUN");
  INS_LIMOGTK_BUILTIN(builtin_g_signal_connect, "G-SIGNAL-CONNECT");
  INS_LIMOGTK_BUILTIN(builtin_gtk_application_new, "GTK-APPLICATION-NEW");
  INS_LIMOGTK_BUILTIN(builtin_gtk_application_window_new, "GTK-APPLICATION-WINDOW-NEW");
  INS_LIMOGTK_BUILTIN(builtin_gtk_widget_show_all, "GTK-WIDGET-SHOW-ALL");
  INS_LIMOGTK_BUILTIN(builtin_gtk_window_set_default_size, "GTK-WINDOW-SET-DEFAULT-SIZE");
  INS_LIMOGTK_BUILTIN(builtin_gtk_window_set_title, "GTK-WINDOW-SET-TITLE");

  #include "gtk-inserts.h"
  #include "limogtk-enums.h"

  setq(env, make_sym("_GTK"), limo_limogtk_env);
}
