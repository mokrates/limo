#include "limo_ncurses.h"

limo_data *sym_ncurses_window;

#define INS_NCURSES_BUILTIN(f, name) setq(limo_ncurses_env, make_sym(name), make_builtin(f))

BUILTIN(builtin_ncurses_stdscr)
{
  return make_special(sym_ncurses_window, (void *)stdscr);
}

BUILTIN(builtin_ncurses_initscr)
{
  initscr();
  return nil;
}

BUILTIN(builtin_ncurses_start_color)
{
  return make_number_from_long_long(start_color());
}

BUILTIN(builtin_ncurses_has_colors)
{
  if (has_colors())
    return sym_true;
  else
    return nil;
}

BUILTIN(builtin_ncurses_init_pair)
{
  limo_data *ld_pair, *ld_fg, *ld_bg;
  unsigned pair, fg, bg;
  REQUIRE_ARGC("INIT-PAIR", 3);
  ld_pair = eval(FIRST_ARG, env);
  ld_fg   = eval(SECOND_ARG, env);
  ld_bg   = eval(THIRD_ARG, env);
  pair = GETINTFROMMPQ(ld_pair);
  fg = GETINTFROMMPQ(ld_fg);
  bg = GETINTFROMMPQ(ld_bg);
  return make_number_from_long_long(init_pair(pair, fg, bg));
}

BUILTIN(builtin_ncurses_color_pair)
{
  limo_data *ld_pnum;
  unsigned pnum;
  REQUIRE_ARGC("COLOR-PAIR", 1);
  ld_pnum = eval(FIRST_ARG, env);
  pnum = GETINTFROMMPQ(ld_pnum);
  return make_number_from_long_long(COLOR_PAIR(pnum));
}

BUILTIN(builtin_ncurses_keypad)
{
  limo_data *ld_win, *ld_bf;
  WINDOW *win;
  int bf;
  REQUIRE_ARGC("KEYPAD", 2);
  ld_win = eval(FIRST_ARG, env);
  ld_bf = eval(SECOND_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  if (is_nil(ld_bf))
    bf=0;
  else
    bf=1;
  return make_number_from_long_long(keypad(win, bf));
}

BUILTIN(builtin_ncurses_nodelay)
{
  limo_data *ld_win, *ld_bf;
  WINDOW *win;
  int bf;
  REQUIRE_ARGC("NODELAY", 2);
  ld_win = eval(FIRST_ARG, env);
  ld_bf = eval(SECOND_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  if (is_nil(ld_bf))
    bf=0;
  else
    bf=1;
  return make_number_from_long_long(nodelay(win, bf));
}

BUILTIN(builtin_ncurses_halfdelay)
{
  limo_data *ld_tenths;
  REQUIRE_ARGC("HALFDELAY", 1);
  ld_tenths = eval(FIRST_ARG, env);
  REQUIRE_TYPE("HALFDELAY", ld_tenths, limo_TYPE_GMPQ);
  halfdelay(GETINTFROMMPQ(ld_tenths));
  return nil;
}

// BUILTIN(builtin_ncurses_@(fname))  { @(fname)();  return nil; }
BUILTIN(builtin_ncurses_nonl)     { nonl();     return nil; }
BUILTIN(builtin_ncurses_nl)       { nl();       return nil; }
BUILTIN(builtin_ncurses_nocbreak) { nocbreak(); return nil; }
BUILTIN(builtin_ncurses_cbreak)   { cbreak();   return nil; }
BUILTIN(builtin_ncurses_noraw)    { noraw();    return nil; }
BUILTIN(builtin_ncurses_raw)      { raw();      return nil; }
BUILTIN(builtin_ncurses_echo)     { echo();     return nil; }
BUILTIN(builtin_ncurses_noecho)   { noecho();   return nil; }
BUILTIN(builtin_ncurses_endwin)   { endwin();   return nil; }

BUILTIN(builtin_ncurses_waddch)
{
  limo_data *ld_win, *ld_chr;
  WINDOW *win;
  unsigned chr;
  REQUIRE_ARGC("WADDCH", 2);
  ld_win = eval(FIRST_ARG, env);
  ld_chr = eval(SECOND_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  chr = GETINTFROMMPQ(ld_chr);
  return make_number_from_long_long(waddch(win, chr));
}

BUILTIN(builtin_ncurses_wmove)
{
  limo_data *ld_win, *ld_x, *ld_y;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC("WMOVE", 3);
  ld_win = eval(FIRST_ARG, env);
  ld_x = eval(SECOND_ARG, env);
  ld_y = eval(THIRD_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  x = GETINTFROMMPQ(ld_x);
  y = GETINTFROMMPQ(ld_y);
  return make_number_from_long_long(wmove(win, x, y));
}

BUILTIN(builtin_ncurses_wrefresh)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC("WREFRESH", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_number_from_long_long(wrefresh(win));
}

BUILTIN(builtin_ncurses_wgetch)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC("WGETCH", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_number_from_long_long(wgetch(win));
}

BUILTIN(builtin_ncurses_wclear)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC("WCLEAR", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_number_from_long_long(wclear(win));
}

BUILTIN(builtin_ncurses_werase)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC("WERASE", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_number_from_long_long(werase(win));
}

BUILTIN(builtin_ncurses_getyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC("WGETYX", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getyx(win, y, x);
  return make_cons(make_number_from_long_long(y), make_number_from_long_long(x));
}

BUILTIN(builtin_ncurses_getparyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC("WGETPARYX", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getparyx(win, y, x);
  return make_cons(make_number_from_long_long(y), make_number_from_long_long(x));
}

BUILTIN(builtin_ncurses_getbegyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC("WGETBEGYX", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getbegyx(win, y, x);
  return make_cons(make_number_from_long_long(y), make_number_from_long_long(x));
}

BUILTIN(builtin_ncurses_getmaxyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC("WGETMAXYX", 1);
  ld_win = eval(FIRST_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getmaxyx(win, y, x);
  return make_cons(make_number_from_long_long(y), make_number_from_long_long(x));
}

BUILTIN(builtin_ncurses_wattron)
{
  limo_data *ld_win, *ld_attr;
  WINDOW *win;
  unsigned attr;
  REQUIRE_ARGC("WATTRON", 2);
  ld_win = eval(FIRST_ARG, env);
  ld_attr= eval(SECOND_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  attr = GETINTFROMMPQ(ld_attr);
  return make_number_from_long_long(wattron(win, attr));
}

BUILTIN(builtin_ncurses_wattroff)
{
  limo_data *ld_win, *ld_attr;
  WINDOW *win;
  unsigned attr;
  REQUIRE_ARGC("WATTROFF", 2);
  ld_win = eval(FIRST_ARG, env);
  ld_attr= eval(SECOND_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  attr = GETINTFROMMPQ(ld_attr);
  return make_number_from_long_long(wattroff(win, attr));
}

BUILTIN(builtin_ncurses_wattrset)
{
  limo_data *ld_win, *ld_attr;
  WINDOW *win;
  unsigned attr;
  REQUIRE_ARGC("WATTRSET", 2);
  ld_win = eval(FIRST_ARG, env);
  ld_attr= eval(SECOND_ARG, env);
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  attr = GETINTFROMMPQ(ld_attr);
  return make_number_from_long_long(wattrset(win, attr));
}

/* (fset 'builtin_to_insbuiltin
   [?\C-s ?\) return left ?\C-  ?\M-b ?\M-w ?\C-a ?I ?N ?S ?_ ?N ?C ?U ?R ?S ?E ?S ?_ ?B ?U ?I ?L ?T ?I ?N ?\( ?b ?u ?i ?l ?t ?i ?n ?_ ?n ?c ?u ?r ?e backspace ?s ?e ?s ?_ ?\C-y ?, ?  ?\" ?\C-y ?: backspace ?\" ?\) ?l backspace ?\; ?  ?\C-  ?\C-e delete left left left left ?\C-  ?\C-r ?\" ?\M-u ?\C-a ?\C-n])
*/

void limo_init_ncurses(limo_data *env)
{
  limo_data *limo_ncurses_env;
  sym_ncurses_window = make_sym("NCURSES-WINDOW");
  limo_ncurses_env = make_env(nil);

  INS_NCURSES_BUILTIN(builtin_ncurses_stdscr, "STDSCR");
  INS_NCURSES_BUILTIN(builtin_ncurses_initscr, "INITSCR");
  INS_NCURSES_BUILTIN(builtin_ncurses_start_color, "START-COLOR");
  INS_NCURSES_BUILTIN(builtin_ncurses_has_colors, "HAS-COLORS");
  INS_NCURSES_BUILTIN(builtin_ncurses_init_pair, "INIT-PAIR"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_color_pair, "COLOR-PAIR"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_keypad, "KEYPAD");
  INS_NCURSES_BUILTIN(builtin_ncurses_nodelay, "NODELAY");
  INS_NCURSES_BUILTIN(builtin_ncurses_halfdelay, "HALFDELAY");
  INS_NCURSES_BUILTIN(builtin_ncurses_nonl, "NONL"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_nl, "NL"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_nocbreak, "NOCBREAK"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_cbreak, "CBREAK"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_noraw, "NORAW"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_raw, "RAW"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_noecho, "NOECHO");
  INS_NCURSES_BUILTIN(builtin_ncurses_echo, "ECHO");
  INS_NCURSES_BUILTIN(builtin_ncurses_endwin, "ENDWIN"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_waddch, "WADDCH"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_wmove, "WMOVE");
  INS_NCURSES_BUILTIN(builtin_ncurses_wrefresh, "WREFRESH"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_wgetch, "WGETCH");
  INS_NCURSES_BUILTIN(builtin_ncurses_wclear, "WCLEAR");
  INS_NCURSES_BUILTIN(builtin_ncurses_werase, "WERASE");
  INS_NCURSES_BUILTIN(builtin_ncurses_getyx, "GETYX"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_getparyx, "GETPARYX"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_getbegyx, "GETBEGYX"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_getmaxyx, "GETMAXYX"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_wattron, "WATTRON"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_wattroff, "WATTROFF"); 
  INS_NCURSES_BUILTIN(builtin_ncurses_wattrset, "WATTRSET"); 
  
  // Attributes
  setq(limo_ncurses_env, make_sym("A_NORMAL"), make_number_from_long_long(A_NORMAL));
  setq(limo_ncurses_env, make_sym("A_ATTRIBUTES"), make_number_from_long_long(A_ATTRIBUTES));
  setq(limo_ncurses_env, make_sym("A_CHARTEXT"), make_number_from_long_long(A_CHARTEXT));
  setq(limo_ncurses_env, make_sym("A_COLOR"), make_number_from_long_long(A_COLOR));
  setq(limo_ncurses_env, make_sym("A_STANDOUT"), make_number_from_long_long(A_STANDOUT));
  setq(limo_ncurses_env, make_sym("A_UNDERLINE"), make_number_from_long_long(A_UNDERLINE));
  setq(limo_ncurses_env, make_sym("A_REVERSE"), make_number_from_long_long(A_REVERSE));
  setq(limo_ncurses_env, make_sym("A_BLINK"), make_number_from_long_long(A_BLINK));
  setq(limo_ncurses_env, make_sym("A_DIM"), make_number_from_long_long(A_DIM));
  setq(limo_ncurses_env, make_sym("A_BOLD"), make_number_from_long_long(A_BOLD));
  setq(limo_ncurses_env, make_sym("A_ALTCHARSET"), make_number_from_long_long(A_ALTCHARSET));
  setq(limo_ncurses_env, make_sym("A_INVIS"), make_number_from_long_long(A_INVIS));
  setq(limo_ncurses_env, make_sym("A_PROTECT"), make_number_from_long_long(A_PROTECT));
  setq(limo_ncurses_env, make_sym("A_HORIZONTAL"), make_number_from_long_long(A_HORIZONTAL));
  setq(limo_ncurses_env, make_sym("A_LEFT"), make_number_from_long_long(A_LEFT));
  setq(limo_ncurses_env, make_sym("A_LOW"), make_number_from_long_long(A_LOW));
  setq(limo_ncurses_env, make_sym("A_RIGHT"), make_number_from_long_long(A_RIGHT));
  setq(limo_ncurses_env, make_sym("A_TOP"), make_number_from_long_long(A_TOP));
  setq(limo_ncurses_env, make_sym("A_VERTICAL"), make_number_from_long_long(A_VERTICAL));

  // keycodes
  setq(limo_ncurses_env, make_sym("KEY_CODE_YES"), make_number_from_long_long(KEY_CODE_YES));
  setq(limo_ncurses_env, make_sym("KEY_MIN"), make_number_from_long_long(KEY_MIN));
  setq(limo_ncurses_env, make_sym("KEY_BREAK"), make_number_from_long_long(KEY_BREAK));
  setq(limo_ncurses_env, make_sym("KEY_SRESET"), make_number_from_long_long(KEY_SRESET));
  setq(limo_ncurses_env, make_sym("KEY_RESET"), make_number_from_long_long(KEY_RESET));

  setq(limo_ncurses_env, make_sym("KEY_DOWN"), make_number_from_long_long(KEY_DOWN));
  setq(limo_ncurses_env, make_sym("KEY_UP"), make_number_from_long_long(KEY_UP));
  setq(limo_ncurses_env, make_sym("KEY_LEFT"), make_number_from_long_long(KEY_LEFT));
  setq(limo_ncurses_env, make_sym("KEY_RIGHT"), make_number_from_long_long(KEY_RIGHT));
  setq(limo_ncurses_env, make_sym("KEY_HOME"), make_number_from_long_long(KEY_HOME));
  setq(limo_ncurses_env, make_sym("KEY_BACKSPACE"), make_number_from_long_long(KEY_BACKSPACE));
  setq(limo_ncurses_env, make_sym("KEY_F0"), make_number_from_long_long(KEY_F0));

  //setq(limo_ncurses_env, make_sym("KEY_F@[1]"), make_number_from_long_long(KEY_F(@[1])));
  setq(limo_ncurses_env, make_sym("KEY_F1"), make_number_from_long_long(KEY_F(1)));
  setq(limo_ncurses_env, make_sym("KEY_F2"), make_number_from_long_long(KEY_F(2)));
  setq(limo_ncurses_env, make_sym("KEY_F3"), make_number_from_long_long(KEY_F(3)));
  setq(limo_ncurses_env, make_sym("KEY_F4"), make_number_from_long_long(KEY_F(4)));
  setq(limo_ncurses_env, make_sym("KEY_F5"), make_number_from_long_long(KEY_F(5)));
  setq(limo_ncurses_env, make_sym("KEY_F6"), make_number_from_long_long(KEY_F(6)));
  setq(limo_ncurses_env, make_sym("KEY_F7"), make_number_from_long_long(KEY_F(7)));
  setq(limo_ncurses_env, make_sym("KEY_F8"), make_number_from_long_long(KEY_F(8)));
  setq(limo_ncurses_env, make_sym("KEY_F9"), make_number_from_long_long(KEY_F(9)));
  setq(limo_ncurses_env, make_sym("KEY_F10"), make_number_from_long_long(KEY_F(10)));
  setq(limo_ncurses_env, make_sym("KEY_F11"), make_number_from_long_long(KEY_F(11)));
  setq(limo_ncurses_env, make_sym("KEY_F12"), make_number_from_long_long(KEY_F(12)));
  
  setq(limo_ncurses_env, make_sym("KEY_DL"), make_number_from_long_long(KEY_DL));
  setq(limo_ncurses_env, make_sym("KEY_IL"), make_number_from_long_long(KEY_IL));
  setq(limo_ncurses_env, make_sym("KEY_DC"), make_number_from_long_long(KEY_DC));
  setq(limo_ncurses_env, make_sym("KEY_IC"), make_number_from_long_long(KEY_IC));
  setq(limo_ncurses_env, make_sym("KEY_EIC"), make_number_from_long_long(KEY_EIC));
  setq(limo_ncurses_env, make_sym("KEY_CLEAR"), make_number_from_long_long(KEY_CLEAR));
  setq(limo_ncurses_env, make_sym("KEY_EOS"), make_number_from_long_long(KEY_EOS));
  setq(limo_ncurses_env, make_sym("KEY_EOL"), make_number_from_long_long(KEY_EOL));
  setq(limo_ncurses_env, make_sym("KEY_SF"), make_number_from_long_long(KEY_SF));
  setq(limo_ncurses_env, make_sym("KEY_SR"), make_number_from_long_long(KEY_SR));
  setq(limo_ncurses_env, make_sym("KEY_NPAGE"), make_number_from_long_long(KEY_NPAGE));
  setq(limo_ncurses_env, make_sym("KEY_PPAGE"), make_number_from_long_long(KEY_PPAGE));
  setq(limo_ncurses_env, make_sym("KEY_STAB"), make_number_from_long_long(KEY_STAB));
  setq(limo_ncurses_env, make_sym("KEY_CTAB"), make_number_from_long_long(KEY_CTAB));
  setq(limo_ncurses_env, make_sym("KEY_CATAB"), make_number_from_long_long(KEY_CATAB));
  setq(limo_ncurses_env, make_sym("KEY_ENTER"), make_number_from_long_long(KEY_ENTER));
  setq(limo_ncurses_env, make_sym("KEY_PRINT"), make_number_from_long_long(KEY_PRINT));
  setq(limo_ncurses_env, make_sym("KEY_LL"), make_number_from_long_long(KEY_LL));
  setq(limo_ncurses_env, make_sym("KEY_A1"), make_number_from_long_long(KEY_A1));
  setq(limo_ncurses_env, make_sym("KEY_A3"), make_number_from_long_long(KEY_A3));
  setq(limo_ncurses_env, make_sym("KEY_B2"), make_number_from_long_long(KEY_B2));
  setq(limo_ncurses_env, make_sym("KEY_C1"), make_number_from_long_long(KEY_C1));
  setq(limo_ncurses_env, make_sym("KEY_C3"), make_number_from_long_long(KEY_C3));
  setq(limo_ncurses_env, make_sym("KEY_BTAB"), make_number_from_long_long(KEY_BTAB));
  setq(limo_ncurses_env, make_sym("KEY_BEG"), make_number_from_long_long(KEY_BEG));
  setq(limo_ncurses_env, make_sym("KEY_CANCEL"), make_number_from_long_long(KEY_CANCEL));
  setq(limo_ncurses_env, make_sym("KEY_CLOSE"), make_number_from_long_long(KEY_CLOSE));
  setq(limo_ncurses_env, make_sym("KEY_COMMAND"), make_number_from_long_long(KEY_COMMAND));
  setq(limo_ncurses_env, make_sym("KEY_COPY"), make_number_from_long_long(KEY_COPY));
  setq(limo_ncurses_env, make_sym("KEY_CREATE"), make_number_from_long_long(KEY_CREATE));
  setq(limo_ncurses_env, make_sym("KEY_END"), make_number_from_long_long(KEY_END));
  setq(limo_ncurses_env, make_sym("KEY_EXIT"), make_number_from_long_long(KEY_EXIT));
  setq(limo_ncurses_env, make_sym("KEY_FIND"), make_number_from_long_long(KEY_FIND));
  setq(limo_ncurses_env, make_sym("KEY_HELP"), make_number_from_long_long(KEY_HELP));
  setq(limo_ncurses_env, make_sym("KEY_MARK"), make_number_from_long_long(KEY_MARK));
  setq(limo_ncurses_env, make_sym("KEY_MESSAGE"), make_number_from_long_long(KEY_MESSAGE));
  setq(limo_ncurses_env, make_sym("KEY_MOVE"), make_number_from_long_long(KEY_MOVE));
  setq(limo_ncurses_env, make_sym("KEY_NEXT"), make_number_from_long_long(KEY_NEXT));
  setq(limo_ncurses_env, make_sym("KEY_OPEN"), make_number_from_long_long(KEY_OPEN));
  setq(limo_ncurses_env, make_sym("KEY_OPTIONS"), make_number_from_long_long(KEY_OPTIONS));
  setq(limo_ncurses_env, make_sym("KEY_PREVIOUS"), make_number_from_long_long(KEY_PREVIOUS));
  setq(limo_ncurses_env, make_sym("KEY_REDO"), make_number_from_long_long(KEY_REDO));
  setq(limo_ncurses_env, make_sym("KEY_REFERENCE"), make_number_from_long_long(KEY_REFERENCE));
  setq(limo_ncurses_env, make_sym("KEY_REFRESH"), make_number_from_long_long(KEY_REFRESH));
  setq(limo_ncurses_env, make_sym("KEY_REPLACE"), make_number_from_long_long(KEY_REPLACE));
  setq(limo_ncurses_env, make_sym("KEY_RESTART"), make_number_from_long_long(KEY_RESTART));
  setq(limo_ncurses_env, make_sym("KEY_RESUME"), make_number_from_long_long(KEY_RESUME));
  setq(limo_ncurses_env, make_sym("KEY_SAVE"), make_number_from_long_long(KEY_SAVE));
  setq(limo_ncurses_env, make_sym("KEY_SBEG"), make_number_from_long_long(KEY_SBEG));
  setq(limo_ncurses_env, make_sym("KEY_SCANCEL"), make_number_from_long_long(KEY_SCANCEL));
  setq(limo_ncurses_env, make_sym("KEY_SCOMMAND"), make_number_from_long_long(KEY_SCOMMAND));
  setq(limo_ncurses_env, make_sym("KEY_SCOPY"), make_number_from_long_long(KEY_SCOPY));
  setq(limo_ncurses_env, make_sym("KEY_SCREATE"), make_number_from_long_long(KEY_SCREATE));
  setq(limo_ncurses_env, make_sym("KEY_SDC"), make_number_from_long_long(KEY_SDC));
  setq(limo_ncurses_env, make_sym("KEY_SDL"), make_number_from_long_long(KEY_SDL));
  setq(limo_ncurses_env, make_sym("KEY_SELECT"), make_number_from_long_long(KEY_SELECT));
  setq(limo_ncurses_env, make_sym("KEY_SEND"), make_number_from_long_long(KEY_SEND));
  setq(limo_ncurses_env, make_sym("KEY_SEOL"), make_number_from_long_long(KEY_SEOL));
  setq(limo_ncurses_env, make_sym("KEY_SEXIT"), make_number_from_long_long(KEY_SEXIT));
  setq(limo_ncurses_env, make_sym("KEY_SFIND"), make_number_from_long_long(KEY_SFIND));
  setq(limo_ncurses_env, make_sym("KEY_SHELP"), make_number_from_long_long(KEY_SHELP));
  setq(limo_ncurses_env, make_sym("KEY_SHOME"), make_number_from_long_long(KEY_SHOME));
  setq(limo_ncurses_env, make_sym("KEY_SIC"), make_number_from_long_long(KEY_SIC));
  setq(limo_ncurses_env, make_sym("KEY_SLEFT"), make_number_from_long_long(KEY_SLEFT));
  setq(limo_ncurses_env, make_sym("KEY_SMESSAGE"), make_number_from_long_long(KEY_SMESSAGE));
  setq(limo_ncurses_env, make_sym("KEY_SMOVE"), make_number_from_long_long(KEY_SMOVE));
  setq(limo_ncurses_env, make_sym("KEY_SNEXT"), make_number_from_long_long(KEY_SNEXT));
  setq(limo_ncurses_env, make_sym("KEY_SOPTIONS"), make_number_from_long_long(KEY_SOPTIONS));
  setq(limo_ncurses_env, make_sym("KEY_SPREVIOUS"), make_number_from_long_long(KEY_SPREVIOUS));
  setq(limo_ncurses_env, make_sym("KEY_SPRINT"), make_number_from_long_long(KEY_SPRINT));
  setq(limo_ncurses_env, make_sym("KEY_SREDO"), make_number_from_long_long(KEY_SREDO));
  setq(limo_ncurses_env, make_sym("KEY_SREPLACE"), make_number_from_long_long(KEY_SREPLACE));
  setq(limo_ncurses_env, make_sym("KEY_SRIGHT"), make_number_from_long_long(KEY_SRIGHT));
  setq(limo_ncurses_env, make_sym("KEY_SRSUME"), make_number_from_long_long(KEY_SRSUME));
  setq(limo_ncurses_env, make_sym("KEY_SSAVE"), make_number_from_long_long(KEY_SSAVE));
  setq(limo_ncurses_env, make_sym("KEY_SSUSPEND"), make_number_from_long_long(KEY_SSUSPEND));
  setq(limo_ncurses_env, make_sym("KEY_SUNDO"), make_number_from_long_long(KEY_SUNDO));
  setq(limo_ncurses_env, make_sym("KEY_SUSPEND"), make_number_from_long_long(KEY_SUSPEND));
  setq(limo_ncurses_env, make_sym("KEY_UNDO"), make_number_from_long_long(KEY_UNDO));
  setq(limo_ncurses_env, make_sym("KEY_MOUSE"), make_number_from_long_long(KEY_MOUSE));
  setq(limo_ncurses_env, make_sym("KEY_RESIZE"), make_number_from_long_long(KEY_RESIZE));
  setq(limo_ncurses_env, make_sym("KEY_EVENT"), make_number_from_long_long(KEY_EVENT));
  setq(limo_ncurses_env, make_sym("KEY_MAX"), make_number_from_long_long(KEY_MAX));

  // Colors
  setq(limo_ncurses_env, make_sym("COLOR_BLACK"), make_number_from_long_long(COLOR_BLACK));
  setq(limo_ncurses_env, make_sym("COLOR_RED"), make_number_from_long_long(COLOR_RED));
  setq(limo_ncurses_env, make_sym("COLOR_GREEN"), make_number_from_long_long(COLOR_GREEN));
  setq(limo_ncurses_env, make_sym("COLOR_YELLOW"), make_number_from_long_long(COLOR_YELLOW));
  setq(limo_ncurses_env, make_sym("COLOR_BLUE"), make_number_from_long_long(COLOR_BLUE));
  setq(limo_ncurses_env, make_sym("COLOR_MAGENTA"), make_number_from_long_long(COLOR_MAGENTA));
  setq(limo_ncurses_env, make_sym("COLOR_CYAN"), make_number_from_long_long(COLOR_CYAN));
  setq(limo_ncurses_env, make_sym("COLOR_WHITE"), make_number_from_long_long(COLOR_WHITE));

  setq(env, make_sym("_NCURSES"), limo_ncurses_env);
}
