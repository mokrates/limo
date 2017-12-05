#include <vigra_c.h>

extern "C" {
#include <limo.h>
#include "../cvector/cvector.h"

static limo_data *limo_true;
static limo_data *cvector_type_sym;

static inline double make_double_from_number(limo_data *n)
{
  double d;
  d = mpq_get_d(LIMO_MPQ(n));
}

BUILTIN(builtin_vigra_imagewidth_c)
{
  limo_data *filename;
  REQUIRE_ARGC("vigra-imagewidth-c", 1);
  filename = eval(FIRST_ARG, env);
  if (filename->type != limo_TYPE_STRING)
    limo_error("vigra-imagewidth-c requires a string");
  return make_number_from_long_long(vigra_imagewidth_c(filename->data.d_string));
}

BUILTIN(builtin_vigra_imageheight_c)
{
  limo_data *filename;
  REQUIRE_ARGC("vigra-imageheight-c", 1);
  filename = eval(FIRST_ARG, env);
  if (filename->type != limo_TYPE_STRING)
    limo_error("vigra-imageheight-c requires a string");
  return make_number_from_long_long(vigra_imageheight_c(filename->data.d_string));
}

BUILTIN(builtin_vigra_imagenumbands_c)
{
  limo_data *filename;
  REQUIRE_ARGC("vigra-imagenumbands-c", 1);
  filename = eval(FIRST_ARG, env);
  if (filename->type != limo_TYPE_STRING)
    limo_error("vigra-imagenumbands-c requires a string");
  return make_number_from_long_long(vigra_imagenumbands_c(filename->data.d_string));
}

BUILTIN(builtin_vigra_imagenumextrabands_c)
{
  limo_data *filename;
  REQUIRE_ARGC("vigra-imagenumextrabands-c", 1);
  filename = eval(FIRST_ARG, env);
  if (filename->type != limo_TYPE_STRING)
    limo_error("vigra-imagenumextrabands-c requires a string");
  return make_number_from_long_long(vigra_imagenumextrabands_c(filename->data.d_string));
}

void limo_dll_init(limo_data *env) 
{
  cvector_type_sym = make_sym("IMAGE");
  limo_true = make_sym(":T");
  INSBUILTIN(builtin_vigra_imagewidth_c, "VIGRA-IMAGEWIDTH-C");
  INSBUILTIN(builtin_vigra_imageheight_c, "VIGRA-IMAGEHEIGHT-C");
  INSBUILTIN(builtin_vigra_imagenumbands_c, "VIGRA-IMAGENUMBANDS-C");
  INSBUILTIN(builtin_vigra_imagenumextrabands_c, "VIGRA-IMAGENUMEXTRABANDS-C");
}

}
