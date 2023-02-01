#include "limo_sdl.h"

BUILTIN(builtin_sdl_pollevent)
{
  SDL_Event *ev;
  int res;

  ev = (SDL_Event *)GC_malloc(sizeof (SDL_Event));
  res = SDL_PollEvent(ev);
  if (res)
    return make_special(sym_sdl_event, ev);
  else
    return nil;
}

BUILTIN(builtin_sdl_event)
{
  limo_data *ld_ev;
  SDL_Event *ev;
  
  ld_ev = eval(FIRST_ARG, env);
  ev = get_special(ld_ev, sym_sdl_event);

  SDL_ActiveEvent *sdl_activeevent;
  SDL_KeyboardEvent *sdl_keyboardevent;
  SDL_MouseMotionEvent *sdl_mousemotionevent;
  SDL_MouseButtonEvent *sdl_mousebuttonevent;
  SDL_JoyAxisEvent *sdl_joyaxisevent;
  SDL_JoyBallEvent *sdl_joyballevent;
  SDL_JoyHatEvent *sdl_joyhatevent;
  SDL_JoyButtonEvent *sdl_joybuttonevent;
  SDL_QuitEvent *sdl_quitevent;
  SDL_SysWMEvent *sdl_syswmevent;
  SDL_ResizeEvent *sdl_resizeevent;
  SDL_ExposeEvent *sdl_exposeevent;
  SDL_UserEvent *sdl_userevent;

  limo_data *reslist;  // alist with return values

  /* @{0}: @{1|(downcase input)} = ev;
     break; */
  switch (ev->type) {
  case SDL_ACTIVEEVENT: sdl_activeevent = (SDL_ActiveEvent *)ev;
    return make_cons(make_rational_from_long_long(ev->type),
                     make_cons(make_rational_from_long_long(sdl_activeevent->gain),
                               make_cons(make_rational_from_long_long(sdl_activeevent->state), nil)));
    break;
  case SDL_KEYDOWN: 
  case SDL_KEYUP: sdl_keyboardevent = (SDL_KeyboardEvent *)ev;
    break;
  case SDL_MOUSEMOTION: sdl_mousemotionevent = (SDL_MouseMotionEvent *)ev;
    break;
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP: sdl_mousebuttonevent = (SDL_MouseButtonEvent *)ev;
    break;
  case SDL_JOYAXISMOTION: sdl_joyaxisevent = (SDL_JoyAxisEvent *)ev;
    break;
  case SDL_JOYBALLMOTION: sdl_joyballevent = (SDL_JoyBallEvent *)ev;
    break;
  case SDL_JOYHATMOTION: sdl_joyhatevent = (SDL_JoyHatEvent *)ev;
    break;
  case SDL_JOYBUTTONDOWN:
  case SDL_JOYBUTTONUP: sdl_joybuttonevent = (SDL_JoyButtonEvent *)ev;
    break;
  case SDL_QUIT: sdl_quitevent = (SDL_QuitEvent *)ev;
    break;
  case SDL_SYSWMEVENT: sdl_syswmevent = (SDL_SysWMEvent *)ev;
    break;
  case SDL_VIDEORESIZE: sdl_resizeevent = (SDL_ResizeEvent *)ev;
    break;
  case SDL_VIDEOEXPOSE: sdl_exposeevent = (SDL_ExposeEvent *)ev;
    break;
  case SDL_USEREVENT: sdl_userevent = (SDL_UserEvent *)ev;
    break;
  }
}
