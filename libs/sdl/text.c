#include "limo_sdl.h"

BUILTIN(builtin_sdl_ttf_init)
{
  if (TTF_Init())
    throw(make_cons(sym_sdl_error, make_string("TTF-INIT initialization error")));
  return nil;
}

BUILTIN(builtin_sdl_ttf_quit)
{
  TTF_Quit();
}

BUILTIN(builtin_sdl_ttf_wasinit)
{
  if (TTF_WasInit())
    return sym_true;
  else
    return nil;
}

BUILTIN(builtin_sdl_ttf_openfont)
{
  limo_data *ld_file, *ld_ptsize;
  char *filename;
  int ptsize;
  TTF_Font *ttf_fnt;

  REQUIRE_ARGC("SDL-TTF-OPENFONT", 2);
  ld_file = eval(FIRST_ARG, env);
  ld_ptsize=eval(SECOND_ARG, env);
  filename = ld_file->d_string;
  ptsize = GETINTFROMMPQ(ld_ptsize);
  if (!(ttf_fnt = TTF_OpenFont(filename, ptsize)))
    throw(make_cons(sym_sdl_error, make_string("SDL-TTF-OPENFONT: could not open font-file")));
  return make_special(sym_sdl_font, (void *)ttf_fnt);
}

BUILTIN(builtin_sdl_ttf_sizetext)
{
  limo_data *ld_font, *ld_text;
  char *text;
  TTF_Font *font;
  int x, y;
  
  REQUIRE_ARGC("SDL-TTF-SIZETEXT", 2);
  ld_font = eval(FIRST_ARG, env);
  ld_text = eval(SECOND_ARG, env);
  text = ld_text->d_string;
  font = get_special(ld_font, sym_sdl_font);
  if (TTF_SizeText(font, text, &x, &y))
    throw(make_cons(sym_sdl_error, make_string("SDL-TTF-SIZETEXT error")));
  return make_cons(make_number_from_long_long(x), make_number_from_long_long(y));
}

BUILTIN(builtin_sdl_ttf_rendertext_blended)
{
  limo_data *ld_color, *ld_font, *ld_text;
  SDL_Color color;
  TTF_Font *font;
  char *text;
  SDL_Surface *ss_dest;
  REQUIRE_ARGC("SDL-TTF-RENDERTEXT-BLENDED", 3);

  ld_font = eval(FIRST_ARG, env);
  ld_text = eval(SECOND_ARG, env);
  ld_color= eval(THIRD_ARG, env);
  font = get_special(ld_font, sym_sdl_font);
  text = ld_text->d_string;
  color= *(SDL_Color *)get_special(ld_color, sym_sdl_color);
  ss_dest = TTF_RenderText_Blended(font, text, color);
  if (!ss_dest)
    throw(make_cons(sym_sdl_error, make_string("SDL-RENDERTEXT-BLENDED: error")));
  return make_special(sym_sdl_surface, ss_dest);
}
