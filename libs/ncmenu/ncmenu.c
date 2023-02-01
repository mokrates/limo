#include <limo.h>
#include <menu.h>

limo_data *sym_ncmenu_menu;
limo_data *sym_ncmenu_item;
limo_data *sym_ncmenu_error;

#define INS_NCMENU_BUILTIN(f, name) setq(limo_ncmenu_env, make_sym(name), make_builtin(f))

       /* current_item           menu_current(3MENU) */
BUILTIN(builtin_ncmenu_current_item)
{
  limo_data *ld_menu;
  MENU *menu;
  ITEM *result;
  REQUIRE_ARGC("CURRENT-ITEM", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  result = current_item(menu);
  if (!result)
    return nil;
  else
    return make_special(sym_ncmenu_item, result);
}

/* free_item              menu_new(3MENU) */
// wouldn't need this - use GC_register_finalizer

/* free_menu              new(3MENU) */
// wouldn't need this - use GC_register_finalizer

       /* item_count             items(3MENU) */
BUILTIN(builtin_ncmenu_item_count)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("ITEM-COUNT", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(item_count(menu));
}

       /* item_description       menu_name(3MENU) */
BUILTIN(builtin_ncmenu_item_description)
{
  limo_data *ld_item;
  ITEM *item;
  char *desc;
  REQUIRE_ARGC("ITEM-DESCRIPTION", 1);
  ld_item = eval(FIRST_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  desc = item_description(item);
  if (desc)
    return make_string(desc);
  else
    return make_string("");
}

       /* item_index             menu_current(3MENU) */
BUILTIN(builtin_ncmenu_item_index)
{
  limo_data *ld_item;
  ITEM *item;
  REQUIRE_ARGC("ITEM-INDEX", 1);
  ld_item = eval(FIRST_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  return make_rational_from_long_long(item_index(item));
}

       /* item_init              hook(3MENU) */
// for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* item_name              menu_name(3MENU) */
BUILTIN(builtin_ncmenu_item_name)
{
  limo_data *ld_item;
  ITEM *item;
  char *name;
  REQUIRE_ARGC("ITEM-NAME", 1);
  ld_item = eval(FIRST_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  name = item_name(item);
  if (name)
    return make_string(name);
  return make_string("");
}

       /* item_opts              menu_opts(3MENU) */
BUILTIN(builtin_ncmenu_item_opts)
{
  limo_data *ld_item;
  ITEM *item;
  REQUIRE_ARGC("ITEM-OPTS", 1);
  ld_item = eval(FIRST_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  return make_rational_from_long_long(item_opts(item));
}

       /* item_opts_off          menu_opts(3MENU) */
BUILTIN(builtin_ncmenu_item_off)
{
  limo_data *ld_item, *ld_opts;
  ITEM *item;
  int opts;
  REQUIRE_ARGC("ITEM-OPTS-OFF", 2);
  ld_item = eval(FIRST_ARG, env);
  ld_opts = eval(SECOND_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  opts = GETINTFROMMPQ(ld_opts);
  return make_rational_from_long_long(item_opts_off(item, opts));
}

       /* item_opts_on           menu_opts(3MENU) */
BUILTIN(builtin_ncmenu_item_on)
{
  limo_data *ld_item, *ld_opts;
  ITEM *item;
  int opts;
  REQUIRE_ARGC("ITEM-OPTS-ON", 2);
  ld_item = eval(FIRST_ARG, env);
  ld_opts = eval(SECOND_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  opts = GETINTFROMMPQ(ld_opts);
  return make_rational_from_long_long(item_opts_on(item, opts));
}

       /* item_term              hook(3MENU) */
// for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* item_userptr           menu_userptr(3MENU) */
// later - perhaps with hooks

       /* item_value             menu_value(3MENU) */
BUILTIN(builtin_ncmenu_item_value)
{
  limo_data *ld_item;
  ITEM *item;
  REQUIRE_ARGC("ITEM-VALUE", 1);
  ld_item = eval(FIRST_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  if (item_value(item))
    return sym_true;
  else
    return nil;
}

       /* item_visible           menu_visible(3MENU) */
BUILTIN(builtin_ncmenu_item_visible)
{
  limo_data *ld_item;
  ITEM *item;
  REQUIRE_ARGC("ITEM-VISIBLE", 1);
  ld_item = eval(FIRST_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  if (item_visible(item))
    return sym_true;
  else
    return nil;
}

       /* menu_back              attributes(3MENU) */
BUILTIN(builtin_ncmenu_menu_back)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("MENU-BACK", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(menu_back(menu));
}

       /* menu_driver            driver(3MENU) */
BUILTIN(builtin_ncmenu_menu_driver)
{
  limo_data *ld_menu, *ld_c;
  MENU *menu;
  int c;
  REQUIRE_ARGC("MENU-DRIVER", 2);
  ld_menu = eval(FIRST_ARG, env);
  ld_c = eval(SECOND_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  c = GETINTFROMMPQ(ld_c);
  return make_rational_from_long_long(menu_driver(menu, c));
}


       /* menu_fore              attributes(3MENU) */
BUILTIN(builtin_ncmenu_menu_fore)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("MENU-FORE", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(menu_fore(menu));
}

       /* menu_format            format(3MENU) */
       /* menu_grey              attributes(3MENU) */
BUILTIN(builtin_ncmenu_menu_grey)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("MENU-GREY", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(menu_grey(menu));
}


       /* menu_init              hook(3MENU) */
// for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* menu_items             items(3MENU) */
BUILTIN(builtin_ncmenu_menu_items)
{
  limo_data *ld_menu;
  MENU *menu;
  ITEM **items;
  limo_data *res;
  limo_data **next;
  REQUIRE_ARGC("MENU-ITEMS", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  items = menu_items(menu);
  if (!items)
    throw(make_cons(sym_ncmenu_error, make_string("could not get item-list")));

  next = &res;
  for (;*items;items++) {
    *next = make_cons(nil, nil);
    CAR(*next) = make_special(sym_ncmenu_item, *items);
    next = &CDR(*next);
  }
  return res;
}
       /* menu_mark              mark(3MENU) */
       /* menu_opts              opts(3MENU) */
       /* menu_opts_off          opts(3MENU) */
       /* menu_opts_on           opts(3MENU) */
       /* menu_pad               attributes(3MENU) */
BUILTIN(builtin_ncmenu_menu_pad)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("MENU-PAD", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(menu_pad(menu));
}

       /* menu_pattern           pattern(3MENU) */
       /* menu_request_by_name   requestname(3MENU) */
       /* menu_request_name      requestname(3MENU) */
       /* menu_spacing           spacing(3MENU) */
       /* menu_sub               win(3MENU) */

       /* menu_term              hook(3MENU) */
// for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* menu_userptr           userptr(3MENU) */
       /* menu_win               win(3MENU) */
       /* new_item               menu_new(3MENU) */
static void limo_free_item(limo_data *obj, ITEM *user_data)
{
   free(item_name(user_data));
   free(item_description(user_data));
   free_item(user_data);
   //printf("\n(TODO: remove after debug) CALLED FREE_ITEM\n");
}

BUILTIN(builtin_ncmenu_new_item)
{
  limo_data *ld_result, *ld_name, *ld_desc;
  ITEM *item;
  REQUIRE_ARGC("NEW-ITEM", 2);
  ld_name = eval(FIRST_ARG, env);
  ld_desc = eval(SECOND_ARG, env);
  // cannot use GC_strdup here or even just use the strings
  // the ITEM-members aren't searched for references, so this would get collected.
  // freeing is done in the finalizer
  item = new_item(strdup(ld_name->d_string), strdup(ld_desc->d_string)); 
  if (!item)
    throw(make_cons(sym_ncmenu_error, make_string("could not create item")));
  ld_result = make_special(sym_ncmenu_item, item);
  GC_register_finalizer(ld_result, (GC_finalization_proc)limo_free_item, item, NULL, NULL);
  return ld_result;
}
       /* new_menu               new(3MENU) */
static void limo_free_menu(limo_data *obj, MENU *user_data)
{
  free(menu_items(user_data));
  free_menu(user_data);
  //printf("\n(TODO: remove after debug) CALLED FREE_MENU\n");
}

BUILTIN(builtin_ncmenu_new_menu)
{
  limo_data *ld_result, *ld_items;
  MENU *menu;
  ITEM **items;
  int items_length, i;
  
  REQUIRE_ARGC("NEW-MENU", 1);
  ld_items = eval(FIRST_ARG, env);
  items_length = list_length(ld_items);
  items = (ITEM **)malloc((items_length+1)* sizeof (ITEM *));
  for (i=0; !is_nil(ld_items); i++, ld_items=CDR(ld_items))
    items[i] = (ITEM *)get_special(CAR(ld_items), sym_ncmenu_item);
  items[i] = NULL;
  menu = new_menu(items);
  if (!menu)
    throw(make_cons(sym_ncmenu_error, make_string("could not create menu")));
  ld_result = make_special(sym_ncmenu_menu, menu);
  GC_register_finalizer(ld_result, (GC_finalization_proc)limo_free_menu, menu, NULL, NULL);
  return ld_result;
}


       /* pos_menu_cursor        cursor(3MENU) */
       /* post_menu              post(3MENU) */
BUILTIN(builtin_ncmenu_post_menu)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("POST-MENU", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(post_menu(menu));
}

       /* scale_menu             win(3MENU) */
       /* set_current_item       menu_current(3MENU) */
       /* set_item_init          hook(3MENU) */
// not yet

       /* set_item_opts          menu_opts(3MENU) */
BUILTIN(builtin_ncmenu_set_item_opts)
{
  limo_data *ld_item, *ld_opts;
  ITEM *item;
  int opts;
  REQUIRE_ARGC("SET-ITEM-OPTS", 2);
  ld_item = eval(FIRST_ARG, env);
  ld_opts = eval(SECOND_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  opts = GETINTFROMMPQ(ld_opts);
  return make_rational_from_long_long(set_item_opts(item, opts));
}


       /* set_item_term          hook(3MENU) */
// for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* set_item_userptr       menu_userptr(3MENU) */
// cannot put limo_data in there,
// because it gets collected while still reachable, so, no support yet
       /* set_menu_back          attributes(3MENU) */


       /* set_item_value         menu_value(3MENU) */
BUILTIN(builtin_ncmenu_set_item_value)
{
  limo_data *ld_item, *ld_value;
  ITEM *item;
  int value;
  REQUIRE_ARGC("SET-ITEM-VALUE", 2);
  ld_item = eval(FIRST_ARG, env);
  ld_value = eval(SECOND_ARG, env);
  item = (ITEM *)get_special(ld_item, sym_ncmenu_item);
  value = !is_nil(ld_value);
  return make_rational_from_long_long(set_item_value(item, value));
}
       /* set_menu_back          attributes(3MENU) */
BUILTIN(builtin_ncmenu_set_menu_back)
{
  limo_data *ld_menu, *ld_back;
  MENU *menu;
  int back;
  REQUIRE_ARGC("SET-MENU-BACK", 2);
  ld_menu = eval(FIRST_ARG, env);
  ld_back = eval(SECOND_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  back = GETINTFROMMPQ(ld_back);
  return make_rational_from_long_long(set_menu_back(menu, back));
}

       /* set_menu_fore          attributes(3MENU) */
BUILTIN(builtin_ncmenu_set_menu_fore)
{
  limo_data *ld_menu, *ld_fore;
  MENU *menu;
  int fore;
  REQUIRE_ARGC("SET-MENU-FORE", 2);
  ld_menu = eval(FIRST_ARG, env);
  ld_fore = eval(SECOND_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  fore = GETINTFROMMPQ(ld_fore);
  return make_rational_from_long_long(set_menu_fore(menu, fore));
}

       /* set_menu_format        format(3MENU) */
BUILTIN(builtin_ncmenu_set_menu_format)
{
  limo_data *ld_menu, *ld_rows, *ld_cols;
  MENU *menu;
  int rows, cols;
  REQUIRE_ARGC("SET-MENU-FORMAT", 3);
  ld_menu = eval(FIRST_ARG, env);
  ld_rows = eval(SECOND_ARG, env);
  ld_cols = eval(THIRD_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  rows = GETINTFROMMPQ(ld_rows);
  cols = GETINTFROMMPQ(ld_cols);
  return make_rational_from_long_long(set_menu_format(menu, rows, cols));
}


       /* set_menu_grey          attributes(3MENU) */
BUILTIN(builtin_ncmenu_set_menu_grey)
{
  limo_data *ld_menu, *ld_grey;
  MENU *menu;
  int grey;
  REQUIRE_ARGC("SET-MENU-GREY", 2);
  ld_menu = eval(FIRST_ARG, env);
  ld_grey = eval(SECOND_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  grey = GETINTFROMMPQ(ld_grey);
  return make_rational_from_long_long(set_menu_grey(menu, grey));
}

       /* set_menu_init          hook(3MENU) */
  // for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* set_menu_items         items(3MENU) */
BUILTIN(builtin_ncmenu_set_menu_items)
{
  limo_data *ld_result, *ld_items, *ld_menu;
  MENU *menu;
  ITEM **items;
  int items_length, i;
  
  REQUIRE_ARGC("SET-MENU-ITEMS", 2);
  ld_menu = eval(FIRST_ARG, env);
  ld_items = eval(SECOND_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  items_length = list_length(ld_items);
  items = malloc((items_length +1)* sizeof (ITEM *));
  memset(items, 0, (items_length +1)* sizeof (ITEM *));
  for (i=0; !is_nil(ld_items); i++, ld_items=CDR(ld_items))
    items[i] = get_special(CAR(ld_items), sym_ncmenu_item);
  return make_rational_from_long_long(set_menu_items(menu, items));
  GC_register_finalizer(ld_result, (GC_finalization_proc)limo_free_menu, menu, NULL, NULL);
  return ld_result;
}


       /* set_menu_mark          mark(3MENU) */
       /* set_menu_opts          menu_opts(3MENU) */
       /* set_menu_pad           attributes(3MENU) */
BUILTIN(builtin_ncmenu_set_menu_pad)
{
  limo_data *ld_menu, *ld_pad;
  MENU *menu;
  int pad;
  REQUIRE_ARGC("SET-MENU-PAD", 2);
  ld_menu = eval(FIRST_ARG, env);
  ld_pad = eval(SECOND_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  pad = GETINTFROMMPQ(ld_pad);
  return make_rational_from_long_long(set_menu_pad(menu, pad));
}

       /* set_menu_pattern       pattern(3MENU) */
// later

       /* set_menu_spacing       spacing(3MENU) */
       /* set_menu_sub           win(3MENU) */

       /* set_menu_term          hook(3MENU) */
// for now, I won't support hooks. The hook-func doesn't take user-data
// which I would normally use to pass in a limo_data* for the hook-lambda

       /* set_menu_userptr       userptr(3MENU) */
       /* set_menu_win           win(3MENU) */
       /* set_top_row            menu_current(3MENU) */
       /* top_row                menu_current(3MENU) */
       /* unpost_menu            post(3MENU) */
BUILTIN(builtin_ncmenu_unpost_menu)
{
  limo_data *ld_menu;
  MENU *menu;
  REQUIRE_ARGC("UNPOST-MENU", 1);
  ld_menu = eval(FIRST_ARG, env);
  menu = (MENU *)get_special(ld_menu, sym_ncmenu_menu);
  return make_rational_from_long_long(unpost_menu(menu));
}

void limo_init_ncmenu(limo_data *env)
{
  limo_data *limo_ncmenu_env;
  sym_ncmenu_menu = make_sym("NCMENU-MENU");
  sym_ncmenu_item = make_sym("NCMENU-ITEM");
  sym_ncmenu_error= make_sym("NCMENU-ERROR");
  limo_ncmenu_env = make_env(nil);

  //INS_NCMENU_BUILTIN(builtin_ncurses_stdscr, "STDSCR");
  INS_NCMENU_BUILTIN(builtin_ncmenu_current_item, "CURRENT-ITEM");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_count, "ITEM-COUNT");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_description, "ITEM-DESCRIPTION");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_index, "ITEM-INDEX");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_name, "ITEM-NAME");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_opts, "ITEM-OPTS");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_off, "ITEM-OFF");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_on, "ITEM-ON");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_value, "ITEM-VALUE");
  INS_NCMENU_BUILTIN(builtin_ncmenu_item_visible, "ITEM-VISIBLE");
  INS_NCMENU_BUILTIN(builtin_ncmenu_menu_back, "MENU-BACK");
  INS_NCMENU_BUILTIN(builtin_ncmenu_menu_driver, "MENU-DRIVER");
  INS_NCMENU_BUILTIN(builtin_ncmenu_menu_fore, "MENU-FORE");
  INS_NCMENU_BUILTIN(builtin_ncmenu_menu_grey, "MENU-GREY");
  INS_NCMENU_BUILTIN(builtin_ncmenu_menu_items, "MENU-ITEMS");
  INS_NCMENU_BUILTIN(builtin_ncmenu_menu_pad, "MENU-PAD");
  INS_NCMENU_BUILTIN(builtin_ncmenu_new_item, "NEW-ITEM");
  INS_NCMENU_BUILTIN(builtin_ncmenu_new_menu, "NEW-MENU");
  INS_NCMENU_BUILTIN(builtin_ncmenu_post_menu, "POST-MENU");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_item_opts, "SET-ITEM-OPTS");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_item_value, "SET-ITEM-VALUE");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_menu_back, "SET-MENU-BACK");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_menu_fore, "SET-MENU-FORE");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_menu_format, "SET-MENU-FORMAT");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_menu_grey, "SET-MENU-GREY");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_menu_items, "SET-MENU-ITEMS");
  INS_NCMENU_BUILTIN(builtin_ncmenu_set_menu_pad, "SET-MENU-PAD");
  INS_NCMENU_BUILTIN(builtin_ncmenu_unpost_menu, "UNPOST-MENU");
  
  // Options
  setq(limo_ncmenu_env, make_sym("O_SELECTABLE"), make_rational_from_long_long(O_SELECTABLE));
  setq(limo_ncmenu_env, make_sym("O_ONEVALUE"), make_rational_from_long_long(O_ONEVALUE));

  setq(limo_ncmenu_env, make_sym("REQ_LEFT_ITEM"), make_rational_from_long_long(REQ_LEFT_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_RIGHT_ITEM"), make_rational_from_long_long(REQ_RIGHT_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_UP_ITEM"), make_rational_from_long_long(REQ_UP_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_DOWN_ITEM"), make_rational_from_long_long(REQ_DOWN_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_SCR_ULINE"), make_rational_from_long_long(REQ_SCR_ULINE));
  setq(limo_ncmenu_env, make_sym("REQ_SCR_DLINE"), make_rational_from_long_long(REQ_SCR_DLINE));
  setq(limo_ncmenu_env, make_sym("REQ_SCR_DPAGE"), make_rational_from_long_long(REQ_SCR_DPAGE));
  setq(limo_ncmenu_env, make_sym("REQ_SCR_UPAGE"), make_rational_from_long_long(REQ_SCR_UPAGE));
  setq(limo_ncmenu_env, make_sym("REQ_FIRST_ITEM"), make_rational_from_long_long(REQ_FIRST_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_LAST_ITEM"), make_rational_from_long_long(REQ_LAST_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_NEXT_ITEM"), make_rational_from_long_long(REQ_NEXT_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_PREV_ITEM"), make_rational_from_long_long(REQ_PREV_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_TOGGLE_ITEM"), make_rational_from_long_long(REQ_TOGGLE_ITEM));
  setq(limo_ncmenu_env, make_sym("REQ_CLEAR_PATTERN"), make_rational_from_long_long(REQ_CLEAR_PATTERN));
  setq(limo_ncmenu_env, make_sym("REQ_BACK_PATTERN"), make_rational_from_long_long(REQ_BACK_PATTERN));
  setq(limo_ncmenu_env, make_sym("REQ_NEXT_MATCH"), make_rational_from_long_long(REQ_NEXT_MATCH));
  setq(limo_ncmenu_env, make_sym("REQ_PREV_MATCH"), make_rational_from_long_long(REQ_PREV_MATCH));

  setq(env, make_sym("_NCMENU"), limo_ncmenu_env);
}
