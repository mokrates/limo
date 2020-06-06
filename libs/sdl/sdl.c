#include "limo_sdl.h"

limo_data *sym_sdl_error;
limo_data *sym_sdl_surface;
limo_data *sym_sdl_color;
limo_data *sym_sdl_font;

BUILTIN(builtin_sdl_init)
{
  limo_data *ld_flags;
  REQUIRE_ARGC("SDL-INIT", 1);
  ld_flags = eval(FIRST_ARG, env);
  
  if (SDL_Init(GETINTFROMMPQ(ld_flags)))
    throw(make_cons(sym_sdl_error, make_string("could not initialize SDL")));
  
  return nil;
}

BUILTIN(builtin_sdl_initsubsystem)
{
  limo_data *ld_flags;
  REQUIRE_ARGC("SDL-INITSUBSYSTEM", 1);
  ld_flags = eval(FIRST_ARG, env);
  
  if (SDL_InitSubSystem(GETINTFROMMPQ(ld_flags)))
    throw(make_cons(sym_sdl_error, make_string("could not initialize SDL-subsystem")));
  
  return nil;
}

BUILTIN(builtin_sdl_quit)
{
  SDL_Quit();
  return nil;
}

BUILTIN(builtin_sdl_quitsubsystem)
{
  limo_data *ld_flags;
  REQUIRE_ARGC("SDL-QUITSUBSYSTEM", 1);
  ld_flags = eval(FIRST_ARG, env);
  
  SDL_QuitSubSystem(GETINTFROMMPQ(ld_flags));
  return nil;
}

BUILTIN(builtin_sdl_wasinit)
{
  limo_data *ld_flags;
  REQUIRE_ARGC("SDL-WASINIT", 1);
  ld_flags = eval(FIRST_ARG, env);

  return make_number_from_long_long(SDL_WasInit(GETINTFROMMPQ(ld_flags)));
}

BUILTIN(builtin_sdl_flip)
{
  limo_data *ld_surface;
  REQUIRE_ARGC("SDL-FLIP", 1);
  ld_surface = eval(FIRST_ARG, env);
  if (SDL_Flip(get_special(ld_surface, sym_sdl_surface)))
    throw(make_cons(sym_sdl_error, make_string("SDL: error flipping")));
  return nil;
}

void limo_init_sdl(limo_data *env)
{
  limo_data *limo_sdl_env;
  sym_sdl_error = make_sym("SDL-ERROR");
  sym_sdl_surface = make_sym("SDL-SURFACE");
  sym_sdl_color = make_sym("SDL-COLOR");
  sym_sdl_font = make_sym("SDL-FONT");

  /// constants
  limo_data *limo_SDL_INIT_TIMER = make_number_from_long_long(SDL_INIT_TIMER);
  limo_data *limo_SDL_INIT_AUDIO = make_number_from_long_long(SDL_INIT_AUDIO);
  limo_data *limo_SDL_INIT_VIDEO = make_number_from_long_long(SDL_INIT_VIDEO);
  limo_data *limo_SDL_INIT_CDROM = make_number_from_long_long(SDL_INIT_CDROM);
  limo_data *limo_SDL_INIT_JOYSTICK = make_number_from_long_long(SDL_INIT_JOYSTICK);
  limo_data *limo_SDL_INIT_EVERYTHING = make_number_from_long_long(SDL_INIT_EVERYTHING);
  limo_data *limo_SDL_INIT_NOPARACHUTE = make_number_from_long_long(SDL_INIT_NOPARACHUTE);
  limo_data *limo_SDL_INIT_EVENTTHREAD = make_number_from_long_long(SDL_INIT_EVENTTHREAD);

  limo_data *limo_SDL_SWSURFACE = make_number_from_long_long(SDL_SWSURFACE);
  limo_data *limo_SDL_HWSURFACE = make_number_from_long_long(SDL_HWSURFACE);
  limo_data *limo_SDL_ASYNCBLIT = make_number_from_long_long(SDL_ASYNCBLIT);
  limo_data *limo_SDL_ANYFORMAT = make_number_from_long_long(SDL_ANYFORMAT);
  limo_data *limo_SDL_HWPALETTE = make_number_from_long_long(SDL_HWPALETTE);
  limo_data *limo_SDL_DOUBLEBUF = make_number_from_long_long(SDL_DOUBLEBUF);
  limo_data *limo_SDL_FULLSCREEN = make_number_from_long_long(SDL_FULLSCREEN);
  limo_data *limo_SDL_OPENGL = make_number_from_long_long(SDL_OPENGL);
  limo_data *limo_SDL_OPENGLBLIT = make_number_from_long_long(SDL_OPENGLBLIT);
  limo_data *limo_SDL_RESIZABLE = make_number_from_long_long(SDL_RESIZABLE);

  /// insert constants into environment
  limo_sdl_env = make_env(NULL);
  setq(limo_sdl_env, make_sym("SDL_INIT_TIMER"), limo_SDL_INIT_TIMER);
  setq(limo_sdl_env, make_sym("SDL_INIT_AUDIO"), limo_SDL_INIT_AUDIO);
  setq(limo_sdl_env, make_sym("SDL_INIT_VIDEO"), limo_SDL_INIT_VIDEO);
  setq(limo_sdl_env, make_sym("SDL_INIT_CDROM"), limo_SDL_INIT_CDROM);
  setq(limo_sdl_env, make_sym("SDL_INIT_JOYSTICK"), limo_SDL_INIT_JOYSTICK);
  setq(limo_sdl_env, make_sym("SDL_INIT_EVERYTHING"), limo_SDL_INIT_EVERYTHING);
  setq(limo_sdl_env, make_sym("SDL_INIT_NOPARACHUTE"), limo_SDL_INIT_NOPARACHUTE);
  setq(limo_sdl_env, make_sym("SDL_INIT_EVENTTHREAD"), limo_SDL_INIT_EVENTTHREAD);

  setq(limo_sdl_env, make_sym("SDL_SWSURFACE"), limo_SDL_SWSURFACE);
  setq(limo_sdl_env, make_sym("SDL_HWSURFACE"), limo_SDL_HWSURFACE);
  setq(limo_sdl_env, make_sym("SDL_ASYNCBLIT"), limo_SDL_ASYNCBLIT);
  setq(limo_sdl_env, make_sym("SDL_ANYFORMAT"), limo_SDL_ANYFORMAT);
  setq(limo_sdl_env, make_sym("SDL_HWPALETTE"), limo_SDL_HWPALETTE);
  setq(limo_sdl_env, make_sym("SDL_DOUBLEBUF"), limo_SDL_DOUBLEBUF);
  setq(limo_sdl_env, make_sym("SDL_FULLSCREEN"), limo_SDL_FULLSCREEN);
  setq(limo_sdl_env, make_sym("SDL_OPENGL"), limo_SDL_OPENGL);
  setq(limo_sdl_env, make_sym("SDL_OPENGLBLIT"), limo_SDL_OPENGLBLIT);
  setq(limo_sdl_env, make_sym("SDL_RESIZABLE"), limo_SDL_RESIZABLE);

  setq(limo_sdl_env, make_sym("SDL_ACTIVEEVENT"), make_number_from_long_long(SDL_ACTIVEEVENT));
  setq(limo_sdl_env, make_sym("SDL_KEYDOWN"), make_number_from_long_long(SDL_KEYDOWN));
  setq(limo_sdl_env, make_sym("SDL_KEYUP"), make_number_from_long_long(SDL_KEYUP));
  setq(limo_sdl_env, make_sym("SDL_MOUSEMOTION"), make_number_from_long_long(SDL_MOUSEMOTION));
  setq(limo_sdl_env, make_sym("SDL_MOUSEBUTTONDOWN"), make_number_from_long_long(SDL_MOUSEBUTTONDOWN));
  setq(limo_sdl_env, make_sym("SDL_MOUSEBUTTONUP"), make_number_from_long_long(SDL_MOUSEBUTTONUP));
  setq(limo_sdl_env, make_sym("SDL_JOYAXISMOTION"), make_number_from_long_long(SDL_JOYAXISMOTION));
  setq(limo_sdl_env, make_sym("SDL_JOYBALLMOTION"), make_number_from_long_long(SDL_JOYBALLMOTION));
  setq(limo_sdl_env, make_sym("SDL_JOYHATMOTION"), make_number_from_long_long(SDL_JOYHATMOTION));
  setq(limo_sdl_env, make_sym("SDL_JOYBUTTONDOWN"), make_number_from_long_long(SDL_JOYBUTTONDOWN));
  setq(limo_sdl_env, make_sym("SDL_JOYBUTTONUP"), make_number_from_long_long(SDL_JOYBUTTONUP));
  setq(limo_sdl_env, make_sym("SDL_QUIT"), make_number_from_long_long(SDL_QUIT));
  setq(limo_sdl_env, make_sym("SDL_SYSWMEVENT"), make_number_from_long_long(SDL_SYSWMEVENT));
  setq(limo_sdl_env, make_sym("SDL_VIDEORESIZE"), make_number_from_long_long(SDL_VIDEORESIZE));
  setq(limo_sdl_env, make_sym("SDL_VIDEOEXPOSE"), make_number_from_long_long(SDL_VIDEOEXPOSE));
  setq(limo_sdl_env, make_sym("SDL_USEREVENT"), make_number_from_long_long(SDL_USEREVENT));

  INS_SDL_BUILTIN(builtin_sdl_init, "SDL-INIT");
  INS_SDL_BUILTIN(builtin_sdl_initsubsystem, "SDL-INITSUBSYSTEM");
  INS_SDL_BUILTIN(builtin_sdl_quit, "SDL-QUIT");
  INS_SDL_BUILTIN(builtin_sdl_quitsubsystem, "SDL-QUITSUBSYSTEM");
  INS_SDL_BUILTIN(builtin_sdl_wasinit, "SDL-WASINIT");
  INS_SDL_BUILTIN(builtin_sdl_flip, "SDL-FLIP");
  
  INS_SDL_BUILTIN(builtin_sdl_setvideomode, "SDL-SETVIDEOMODE");

  INS_SDL_BUILTIN(builtin_sdl_surface_rgbpcolor, "SDL-SURFACE-RGBPCOLOR");
  INS_SDL_BUILTIN(builtin_sdl_surface_rgbapcolor, "SDL-SURFACE-RGBAPCOLOR"); 
  INS_SDL_BUILTIN(builtin_sdl_makecolor, "SDL-MAKECOLOR"); // not really a sdl-function
  INS_SDL_BUILTIN(builtin_sdl_plot, "SDL-PLOT"); // not really a sdl-function
  INS_SDL_BUILTIN(builtin_sdl_line, "SDL-LINE"); // not really a sdl-function
  INS_SDL_BUILTIN(builtin_sdl_clearsurface, "CLEARSURFACE"); // not really a sdl-function

  INS_SDL_BUILTIN(builtin_sdl_ttf_init, "SDL-TTF-INIT");
  INS_SDL_BUILTIN(builtin_sdl_ttf_quit, "SDL-TTF-QUIT");
  INS_SDL_BUILTIN(builtin_sdl_ttf_wasinit, "SDL-TTF-WASINIT");
  INS_SDL_BUILTIN(builtin_sdl_ttf_openfont, "SDL-TTF-OPENFONT");
  INS_SDL_BUILTIN(builtin_sdl_ttf_sizetext, "SDL-TTF-SIZETEXT");
  INS_SDL_BUILTIN(builtin_sdl_ttf_rendertext_blended, "SDL-TTF-RENDERTEXT-BLENDED");

  INS_SDL_BUILTIN(builtin_sdl_blitsurface, "SDL-BLITSURFACE");

  //  INS_SDL_BUILTIN(builtin_sdl_pollevent, "SDL-POLL-EVENT");
  // INS_SDL_BUILTIN(builtin_sdl_event_type, "EVENT");

  setq(env, make_sym("_SDL"), limo_sdl_env);
}
