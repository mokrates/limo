#include "limo_ncurses.h"

limo_data *sym_ncurses_window;

#define INS_NCURSES_BUILTIN(f, name) setq(limo_ncurses_env, make_sym(name), make_builtin(f, name))
#define INS_NCURSES_BUILTINFUN(f, name) setq(limo_ncurses_env, make_sym(name), make_builtinfun(f, name))

BUILTINFUN(builtin_ncurses_stdscr)
{
  return make_special(sym_ncurses_window, (void *)stdscr);
}

BUILTINFUN(builtin_ncurses_initscr)
{
  return make_special(sym_ncurses_window, initscr());
}

BUILTINFUN(builtin_ncurses_start_color)
{
  return make_rational_from_long_long(start_color());
}

BUILTINFUN(builtin_ncurses_has_colors)
{
  if (has_colors())
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_ncurses_init_pair)
{
  limo_data *ld_pair, *ld_fg, *ld_bg;
  unsigned pair, fg, bg;
  REQUIRE_ARGC_FUN("INIT-PAIR", 3);
  ld_pair = argv[0];
  ld_fg   = argv[1];
  ld_bg   = argv[2];
  pair = GETINTFROMMPQ(ld_pair);
  fg = GETINTFROMMPQ(ld_fg);
  bg = GETINTFROMMPQ(ld_bg);
  return make_rational_from_long_long(init_pair(pair, fg, bg));
}

BUILTINFUN(builtin_ncurses_color_pair)
{
  limo_data *ld_pnum;
  unsigned pnum;
  REQUIRE_ARGC_FUN("COLOR-PAIR", 1);
  ld_pnum = argv[0];
  pnum = GETINTFROMMPQ(ld_pnum);
  return make_rational_from_long_long(COLOR_PAIR(pnum));
}

BUILTINFUN(builtin_ncurses_keypad)
{
  limo_data *ld_win, *ld_bf;
  WINDOW *win;
  int bf;
  REQUIRE_ARGC_FUN("KEYPAD", 2);
  ld_win = argv[0];
  ld_bf = argv[1];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  if (is_nil(ld_bf))
    bf=0;
  else
    bf=1;
  return make_rational_from_long_long(keypad(win, bf));
}

BUILTINFUN(builtin_ncurses_nodelay)
{
  limo_data *ld_win, *ld_bf;
  WINDOW *win;
  int bf;
  REQUIRE_ARGC_FUN("NODELAY", 2);
  ld_win = argv[0];
  ld_bf = argv[1];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  if (is_nil(ld_bf))
    bf=0;
  else
    bf=1;
  return make_rational_from_long_long(nodelay(win, bf));
}

BUILTINFUN(builtin_ncurses_halfdelay)
{
  limo_data *ld_tenths;
  REQUIRE_ARGC_FUN("HALFDELAY", 1);
  ld_tenths = argv[0];
  REQUIRE_TYPE("HALFDELAY", ld_tenths, limo_TYPE_GMPQ);
  halfdelay(GETINTFROMMPQ(ld_tenths));
  return nil;
}

// BUILTINFUN(builtin_ncurses_@(fname))  { @(fname)();  return nil; }
BUILTINFUN(builtin_ncurses_nonl)     { nonl();     return nil; }
BUILTINFUN(builtin_ncurses_nl)       { nl();       return nil; }
BUILTINFUN(builtin_ncurses_nocbreak) { nocbreak(); return nil; }
BUILTINFUN(builtin_ncurses_cbreak)   { cbreak();   return nil; }
BUILTINFUN(builtin_ncurses_noraw)    { noraw();    return nil; }
BUILTINFUN(builtin_ncurses_raw)      { raw();      return nil; }
BUILTINFUN(builtin_ncurses_echo)     { echo();     return nil; }
BUILTINFUN(builtin_ncurses_noecho)   { noecho();   return nil; }
BUILTINFUN(builtin_ncurses_endwin)   { endwin();   return nil; }

BUILTINFUN(builtin_ncurses_waddch)
{
  limo_data *ld_win, *ld_chr;
  WINDOW *win;
  unsigned chr;
  REQUIRE_ARGC_FUN("WADDCH", 2);
  ld_win = argv[0];
  ld_chr = argv[1];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  chr = GETINTFROMMPQ(ld_chr);
  return make_rational_from_long_long(waddch(win, chr));
}

BUILTINFUN(builtin_ncurses_wmove)
{
  limo_data *ld_win, *ld_x, *ld_y;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC_FUN("WMOVE", 3);
  ld_win = argv[0];
  ld_x = argv[1];
  ld_y = argv[2];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  x = GETINTFROMMPQ(ld_x);
  y = GETINTFROMMPQ(ld_y);
  return make_rational_from_long_long(wmove(win, x, y));
}

BUILTINFUN(builtin_ncurses_wrefresh)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC_FUN("WREFRESH", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_rational_from_long_long(wrefresh(win));
}

BUILTINFUN(builtin_ncurses_wgetch)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC_FUN("WGETCH", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_rational_from_long_long(wgetch(win));
}

BUILTINFUN(builtin_ncurses_wclear)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC_FUN("WCLEAR", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_rational_from_long_long(wclear(win));
}

BUILTINFUN(builtin_ncurses_werase)
{
  limo_data *ld_win;
  WINDOW *win;
  REQUIRE_ARGC_FUN("WERASE", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  return make_rational_from_long_long(werase(win));
}

BUILTINFUN(builtin_ncurses_getyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC_FUN("WGETYX", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getyx(win, y, x);
  return make_cons(make_rational_from_long_long(y), make_rational_from_long_long(x));
}

BUILTINFUN(builtin_ncurses_getparyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC_FUN("WGETPARYX", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getparyx(win, y, x);
  return make_cons(make_rational_from_long_long(y), make_rational_from_long_long(x));
}

BUILTINFUN(builtin_ncurses_getbegyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC_FUN("WGETBEGYX", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getbegyx(win, y, x);
  return make_cons(make_rational_from_long_long(y), make_rational_from_long_long(x));
}

BUILTINFUN(builtin_ncurses_getmaxyx)
{
  limo_data *ld_win;
  WINDOW *win;
  unsigned x, y;
  REQUIRE_ARGC_FUN("WGETMAXYX", 1);
  ld_win = argv[0];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  getmaxyx(win, y, x);
  return make_cons(make_rational_from_long_long(y), make_rational_from_long_long(x));
}

BUILTINFUN(builtin_ncurses_wattron)
{
  limo_data *ld_win, *ld_attr;
  WINDOW *win;
  unsigned attr;
  REQUIRE_ARGC_FUN("WATTRON", 2);
  ld_win = argv[0];
  ld_attr= argv[1];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  attr = GETINTFROMMPQ(ld_attr);
  return make_rational_from_long_long(wattron(win, attr));
}

BUILTINFUN(builtin_ncurses_wattroff)
{
  limo_data *ld_win, *ld_attr;
  WINDOW *win;
  unsigned attr;
  REQUIRE_ARGC_FUN("WATTROFF", 2);
  ld_win = argv[0];
  ld_attr= argv[1];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  attr = GETINTFROMMPQ(ld_attr);
  return make_rational_from_long_long(wattroff(win, attr));
}

BUILTINFUN(builtin_ncurses_wattrset)
{
  limo_data *ld_win, *ld_attr;
  WINDOW *win;
  unsigned attr;
  REQUIRE_ARGC_FUN("WATTRSET", 2);
  ld_win = argv[0];
  ld_attr= argv[1];
  win = (WINDOW *)get_special(ld_win, sym_ncurses_window);
  attr = GETINTFROMMPQ(ld_attr);
  return make_rational_from_long_long(wattrset(win, attr));
}

/* (fset 'builtin_to_insbuiltin
   [?\C-s ?\) return left ?\C-  ?\M-b ?\M-w ?\C-a ?I ?N ?S ?_ ?N ?C ?U ?R ?S ?E ?S ?_ ?B ?U ?I ?L ?T ?I ?N ?\( ?b ?u ?i ?l ?t ?i ?n ?_ ?n ?c ?u ?r ?e backspace ?s ?e ?s ?_ ?\C-y ?, ?  ?\" ?\C-y ?: backspace ?\" ?\) ?l backspace ?\; ?  ?\C-  ?\C-e delete left left left left ?\C-  ?\C-r ?\" ?\M-u ?\C-a ?\C-n])
*/

void limo_init_ncurses(limo_data *env)
{
  limo_data *limo_ncurses_env;
  sym_ncurses_window = make_sym("NCURSES-WINDOW");
  limo_ncurses_env = make_env(nil);

  INS_NCURSES_BUILTINFUN(builtin_ncurses_stdscr, "STDSCR");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_initscr, "INITSCR");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_start_color, "START-COLOR");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_has_colors, "HAS-COLORS");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_init_pair, "INIT-PAIR"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_color_pair, "COLOR-PAIR"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_keypad, "KEYPAD");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_nodelay, "NODELAY");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_halfdelay, "HALFDELAY");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_nonl, "NONL"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_nl, "NL"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_nocbreak, "NOCBREAK"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_cbreak, "CBREAK"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_noraw, "NORAW"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_raw, "RAW"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_noecho, "NOECHO");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_echo, "ECHO");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_endwin, "ENDWIN"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_waddch, "WADDCH"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wmove, "WMOVE");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wrefresh, "WREFRESH"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wgetch, "WGETCH");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wclear, "WCLEAR");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_werase, "WERASE");
  INS_NCURSES_BUILTINFUN(builtin_ncurses_getyx, "GETYX"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_getparyx, "GETPARYX"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_getbegyx, "GETBEGYX"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_getmaxyx, "GETMAXYX"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wattron, "WATTRON"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wattroff, "WATTROFF"); 
  INS_NCURSES_BUILTINFUN(builtin_ncurses_wattrset, "WATTRSET"); 
  
  // Attributes
  setq(limo_ncurses_env, make_sym("A_NORMAL"), make_rational_from_long_long(A_NORMAL));
  setq(limo_ncurses_env, make_sym("A_ATTRIBUTES"), make_rational_from_long_long(A_ATTRIBUTES));
  setq(limo_ncurses_env, make_sym("A_CHARTEXT"), make_rational_from_long_long(A_CHARTEXT));
  setq(limo_ncurses_env, make_sym("A_COLOR"), make_rational_from_long_long(A_COLOR));
  setq(limo_ncurses_env, make_sym("A_STANDOUT"), make_rational_from_long_long(A_STANDOUT));
  setq(limo_ncurses_env, make_sym("A_UNDERLINE"), make_rational_from_long_long(A_UNDERLINE));
  setq(limo_ncurses_env, make_sym("A_REVERSE"), make_rational_from_long_long(A_REVERSE));
  setq(limo_ncurses_env, make_sym("A_BLINK"), make_rational_from_long_long(A_BLINK));
  setq(limo_ncurses_env, make_sym("A_DIM"), make_rational_from_long_long(A_DIM));
  setq(limo_ncurses_env, make_sym("A_BOLD"), make_rational_from_long_long(A_BOLD));
  setq(limo_ncurses_env, make_sym("A_ALTCHARSET"), make_rational_from_long_long(A_ALTCHARSET));
  setq(limo_ncurses_env, make_sym("A_INVIS"), make_rational_from_long_long(A_INVIS));
  setq(limo_ncurses_env, make_sym("A_PROTECT"), make_rational_from_long_long(A_PROTECT));
  setq(limo_ncurses_env, make_sym("A_HORIZONTAL"), make_rational_from_long_long(A_HORIZONTAL));
  setq(limo_ncurses_env, make_sym("A_LEFT"), make_rational_from_long_long(A_LEFT));
  setq(limo_ncurses_env, make_sym("A_LOW"), make_rational_from_long_long(A_LOW));
  setq(limo_ncurses_env, make_sym("A_RIGHT"), make_rational_from_long_long(A_RIGHT));
  setq(limo_ncurses_env, make_sym("A_TOP"), make_rational_from_long_long(A_TOP));
  setq(limo_ncurses_env, make_sym("A_VERTICAL"), make_rational_from_long_long(A_VERTICAL));

  // keycodes
  setq(limo_ncurses_env, make_sym("KEY_CODE_YES"), make_rational_from_long_long(KEY_CODE_YES));
  setq(limo_ncurses_env, make_sym("KEY_MIN"), make_rational_from_long_long(KEY_MIN));
  setq(limo_ncurses_env, make_sym("KEY_BREAK"), make_rational_from_long_long(KEY_BREAK));
  setq(limo_ncurses_env, make_sym("KEY_SRESET"), make_rational_from_long_long(KEY_SRESET));
  setq(limo_ncurses_env, make_sym("KEY_RESET"), make_rational_from_long_long(KEY_RESET));

  setq(limo_ncurses_env, make_sym("KEY_DOWN"), make_rational_from_long_long(KEY_DOWN));
  setq(limo_ncurses_env, make_sym("KEY_UP"), make_rational_from_long_long(KEY_UP));
  setq(limo_ncurses_env, make_sym("KEY_LEFT"), make_rational_from_long_long(KEY_LEFT));
  setq(limo_ncurses_env, make_sym("KEY_RIGHT"), make_rational_from_long_long(KEY_RIGHT));
  setq(limo_ncurses_env, make_sym("KEY_HOME"), make_rational_from_long_long(KEY_HOME));
  setq(limo_ncurses_env, make_sym("KEY_BACKSPACE"), make_rational_from_long_long(KEY_BACKSPACE));
  setq(limo_ncurses_env, make_sym("KEY_F0"), make_rational_from_long_long(KEY_F0));

  //setq(limo_ncurses_env, make_sym("KEY_F@[1]"), make_rational_from_long_long(KEY_F(@[1])));
  setq(limo_ncurses_env, make_sym("KEY_F1"), make_rational_from_long_long(KEY_F(1)));
  setq(limo_ncurses_env, make_sym("KEY_F2"), make_rational_from_long_long(KEY_F(2)));
  setq(limo_ncurses_env, make_sym("KEY_F3"), make_rational_from_long_long(KEY_F(3)));
  setq(limo_ncurses_env, make_sym("KEY_F4"), make_rational_from_long_long(KEY_F(4)));
  setq(limo_ncurses_env, make_sym("KEY_F5"), make_rational_from_long_long(KEY_F(5)));
  setq(limo_ncurses_env, make_sym("KEY_F6"), make_rational_from_long_long(KEY_F(6)));
  setq(limo_ncurses_env, make_sym("KEY_F7"), make_rational_from_long_long(KEY_F(7)));
  setq(limo_ncurses_env, make_sym("KEY_F8"), make_rational_from_long_long(KEY_F(8)));
  setq(limo_ncurses_env, make_sym("KEY_F9"), make_rational_from_long_long(KEY_F(9)));
  setq(limo_ncurses_env, make_sym("KEY_F10"), make_rational_from_long_long(KEY_F(10)));
  setq(limo_ncurses_env, make_sym("KEY_F11"), make_rational_from_long_long(KEY_F(11)));
  setq(limo_ncurses_env, make_sym("KEY_F12"), make_rational_from_long_long(KEY_F(12)));
  
  setq(limo_ncurses_env, make_sym("KEY_DL"), make_rational_from_long_long(KEY_DL));
  setq(limo_ncurses_env, make_sym("KEY_IL"), make_rational_from_long_long(KEY_IL));
  setq(limo_ncurses_env, make_sym("KEY_DC"), make_rational_from_long_long(KEY_DC));
  setq(limo_ncurses_env, make_sym("KEY_IC"), make_rational_from_long_long(KEY_IC));
  setq(limo_ncurses_env, make_sym("KEY_EIC"), make_rational_from_long_long(KEY_EIC));
  setq(limo_ncurses_env, make_sym("KEY_CLEAR"), make_rational_from_long_long(KEY_CLEAR));
  setq(limo_ncurses_env, make_sym("KEY_EOS"), make_rational_from_long_long(KEY_EOS));
  setq(limo_ncurses_env, make_sym("KEY_EOL"), make_rational_from_long_long(KEY_EOL));
  setq(limo_ncurses_env, make_sym("KEY_SF"), make_rational_from_long_long(KEY_SF));
  setq(limo_ncurses_env, make_sym("KEY_SR"), make_rational_from_long_long(KEY_SR));
  setq(limo_ncurses_env, make_sym("KEY_NPAGE"), make_rational_from_long_long(KEY_NPAGE));
  setq(limo_ncurses_env, make_sym("KEY_PPAGE"), make_rational_from_long_long(KEY_PPAGE));
  setq(limo_ncurses_env, make_sym("KEY_STAB"), make_rational_from_long_long(KEY_STAB));
  setq(limo_ncurses_env, make_sym("KEY_CTAB"), make_rational_from_long_long(KEY_CTAB));
  setq(limo_ncurses_env, make_sym("KEY_CATAB"), make_rational_from_long_long(KEY_CATAB));
  setq(limo_ncurses_env, make_sym("KEY_ENTER"), make_rational_from_long_long(KEY_ENTER));
  setq(limo_ncurses_env, make_sym("KEY_PRINT"), make_rational_from_long_long(KEY_PRINT));
  setq(limo_ncurses_env, make_sym("KEY_LL"), make_rational_from_long_long(KEY_LL));
  setq(limo_ncurses_env, make_sym("KEY_A1"), make_rational_from_long_long(KEY_A1));
  setq(limo_ncurses_env, make_sym("KEY_A3"), make_rational_from_long_long(KEY_A3));
  setq(limo_ncurses_env, make_sym("KEY_B2"), make_rational_from_long_long(KEY_B2));
  setq(limo_ncurses_env, make_sym("KEY_C1"), make_rational_from_long_long(KEY_C1));
  setq(limo_ncurses_env, make_sym("KEY_C3"), make_rational_from_long_long(KEY_C3));
  setq(limo_ncurses_env, make_sym("KEY_BTAB"), make_rational_from_long_long(KEY_BTAB));
  setq(limo_ncurses_env, make_sym("KEY_BEG"), make_rational_from_long_long(KEY_BEG));
  setq(limo_ncurses_env, make_sym("KEY_CANCEL"), make_rational_from_long_long(KEY_CANCEL));
  setq(limo_ncurses_env, make_sym("KEY_CLOSE"), make_rational_from_long_long(KEY_CLOSE));
  setq(limo_ncurses_env, make_sym("KEY_COMMAND"), make_rational_from_long_long(KEY_COMMAND));
  setq(limo_ncurses_env, make_sym("KEY_COPY"), make_rational_from_long_long(KEY_COPY));
  setq(limo_ncurses_env, make_sym("KEY_CREATE"), make_rational_from_long_long(KEY_CREATE));
  setq(limo_ncurses_env, make_sym("KEY_END"), make_rational_from_long_long(KEY_END));
  setq(limo_ncurses_env, make_sym("KEY_EXIT"), make_rational_from_long_long(KEY_EXIT));
  setq(limo_ncurses_env, make_sym("KEY_FIND"), make_rational_from_long_long(KEY_FIND));
  setq(limo_ncurses_env, make_sym("KEY_HELP"), make_rational_from_long_long(KEY_HELP));
  setq(limo_ncurses_env, make_sym("KEY_MARK"), make_rational_from_long_long(KEY_MARK));
  setq(limo_ncurses_env, make_sym("KEY_MESSAGE"), make_rational_from_long_long(KEY_MESSAGE));
  setq(limo_ncurses_env, make_sym("KEY_MOVE"), make_rational_from_long_long(KEY_MOVE));
  setq(limo_ncurses_env, make_sym("KEY_NEXT"), make_rational_from_long_long(KEY_NEXT));
  setq(limo_ncurses_env, make_sym("KEY_OPEN"), make_rational_from_long_long(KEY_OPEN));
  setq(limo_ncurses_env, make_sym("KEY_OPTIONS"), make_rational_from_long_long(KEY_OPTIONS));
  setq(limo_ncurses_env, make_sym("KEY_PREVIOUS"), make_rational_from_long_long(KEY_PREVIOUS));
  setq(limo_ncurses_env, make_sym("KEY_REDO"), make_rational_from_long_long(KEY_REDO));
  setq(limo_ncurses_env, make_sym("KEY_REFERENCE"), make_rational_from_long_long(KEY_REFERENCE));
  setq(limo_ncurses_env, make_sym("KEY_REFRESH"), make_rational_from_long_long(KEY_REFRESH));
  setq(limo_ncurses_env, make_sym("KEY_REPLACE"), make_rational_from_long_long(KEY_REPLACE));
  setq(limo_ncurses_env, make_sym("KEY_RESTART"), make_rational_from_long_long(KEY_RESTART));
  setq(limo_ncurses_env, make_sym("KEY_RESUME"), make_rational_from_long_long(KEY_RESUME));
  setq(limo_ncurses_env, make_sym("KEY_SAVE"), make_rational_from_long_long(KEY_SAVE));
  setq(limo_ncurses_env, make_sym("KEY_SBEG"), make_rational_from_long_long(KEY_SBEG));
  setq(limo_ncurses_env, make_sym("KEY_SCANCEL"), make_rational_from_long_long(KEY_SCANCEL));
  setq(limo_ncurses_env, make_sym("KEY_SCOMMAND"), make_rational_from_long_long(KEY_SCOMMAND));
  setq(limo_ncurses_env, make_sym("KEY_SCOPY"), make_rational_from_long_long(KEY_SCOPY));
  setq(limo_ncurses_env, make_sym("KEY_SCREATE"), make_rational_from_long_long(KEY_SCREATE));
  setq(limo_ncurses_env, make_sym("KEY_SDC"), make_rational_from_long_long(KEY_SDC));
  setq(limo_ncurses_env, make_sym("KEY_SDL"), make_rational_from_long_long(KEY_SDL));
  setq(limo_ncurses_env, make_sym("KEY_SELECT"), make_rational_from_long_long(KEY_SELECT));
  setq(limo_ncurses_env, make_sym("KEY_SEND"), make_rational_from_long_long(KEY_SEND));
  setq(limo_ncurses_env, make_sym("KEY_SEOL"), make_rational_from_long_long(KEY_SEOL));
  setq(limo_ncurses_env, make_sym("KEY_SEXIT"), make_rational_from_long_long(KEY_SEXIT));
  setq(limo_ncurses_env, make_sym("KEY_SFIND"), make_rational_from_long_long(KEY_SFIND));
  setq(limo_ncurses_env, make_sym("KEY_SHELP"), make_rational_from_long_long(KEY_SHELP));
  setq(limo_ncurses_env, make_sym("KEY_SHOME"), make_rational_from_long_long(KEY_SHOME));
  setq(limo_ncurses_env, make_sym("KEY_SIC"), make_rational_from_long_long(KEY_SIC));
  setq(limo_ncurses_env, make_sym("KEY_SLEFT"), make_rational_from_long_long(KEY_SLEFT));
  setq(limo_ncurses_env, make_sym("KEY_SMESSAGE"), make_rational_from_long_long(KEY_SMESSAGE));
  setq(limo_ncurses_env, make_sym("KEY_SMOVE"), make_rational_from_long_long(KEY_SMOVE));
  setq(limo_ncurses_env, make_sym("KEY_SNEXT"), make_rational_from_long_long(KEY_SNEXT));
  setq(limo_ncurses_env, make_sym("KEY_SOPTIONS"), make_rational_from_long_long(KEY_SOPTIONS));
  setq(limo_ncurses_env, make_sym("KEY_SPREVIOUS"), make_rational_from_long_long(KEY_SPREVIOUS));
  setq(limo_ncurses_env, make_sym("KEY_SPRINT"), make_rational_from_long_long(KEY_SPRINT));
  setq(limo_ncurses_env, make_sym("KEY_SREDO"), make_rational_from_long_long(KEY_SREDO));
  setq(limo_ncurses_env, make_sym("KEY_SREPLACE"), make_rational_from_long_long(KEY_SREPLACE));
  setq(limo_ncurses_env, make_sym("KEY_SRIGHT"), make_rational_from_long_long(KEY_SRIGHT));
  setq(limo_ncurses_env, make_sym("KEY_SRSUME"), make_rational_from_long_long(KEY_SRSUME));
  setq(limo_ncurses_env, make_sym("KEY_SSAVE"), make_rational_from_long_long(KEY_SSAVE));
  setq(limo_ncurses_env, make_sym("KEY_SSUSPEND"), make_rational_from_long_long(KEY_SSUSPEND));
  setq(limo_ncurses_env, make_sym("KEY_SUNDO"), make_rational_from_long_long(KEY_SUNDO));
  setq(limo_ncurses_env, make_sym("KEY_SUSPEND"), make_rational_from_long_long(KEY_SUSPEND));
  setq(limo_ncurses_env, make_sym("KEY_UNDO"), make_rational_from_long_long(KEY_UNDO));
  setq(limo_ncurses_env, make_sym("KEY_MOUSE"), make_rational_from_long_long(KEY_MOUSE));
  setq(limo_ncurses_env, make_sym("KEY_RESIZE"), make_rational_from_long_long(KEY_RESIZE));
  //setq(limo_ncurses_env, make_sym("KEY_EVENT"), make_rational_from_long_long(KEY_EVENT));
  setq(limo_ncurses_env, make_sym("KEY_MAX"), make_rational_from_long_long(KEY_MAX));

  // Colors
  setq(limo_ncurses_env, make_sym("COLOR_BLACK"), make_rational_from_long_long(COLOR_BLACK));
  setq(limo_ncurses_env, make_sym("COLOR_RED"), make_rational_from_long_long(COLOR_RED));
  setq(limo_ncurses_env, make_sym("COLOR_GREEN"), make_rational_from_long_long(COLOR_GREEN));
  setq(limo_ncurses_env, make_sym("COLOR_YELLOW"), make_rational_from_long_long(COLOR_YELLOW));
  setq(limo_ncurses_env, make_sym("COLOR_BLUE"), make_rational_from_long_long(COLOR_BLUE));
  setq(limo_ncurses_env, make_sym("COLOR_MAGENTA"), make_rational_from_long_long(COLOR_MAGENTA));
  setq(limo_ncurses_env, make_sym("COLOR_CYAN"), make_rational_from_long_long(COLOR_CYAN));
  setq(limo_ncurses_env, make_sym("COLOR_WHITE"), make_rational_from_long_long(COLOR_WHITE));

  setq(env, make_sym("_NCURSES"), limo_ncurses_env);
}
