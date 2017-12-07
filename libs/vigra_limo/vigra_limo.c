//// This preamble is a little ... spaghetti

/// libgmp.h needs to not know we're using c++
#include <vigra_c.h>

extern "C" {
#include <limo.h>
#include "../cvector/cvector.h"
  
  //////// End of Preamble
  
  static limo_data *limo_true;
  static limo_data *cvector_type_sym;
  
  static inline double make_double_from_number(limo_data *n)
  {
    double d;
    d = mpq_get_d(LIMO_MPQ(n));
  }

  static void ensure_cvectorp(limo_data *cv)
  {
    if (cv->type == limo_TYPE_SPECIAL &&
	limo_equals(CAR(cv->data.d_special), cvector_type_sym))
      return;
    else
      limo_error("CVECTOR expected");
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

  BUILTIN(builtin_vigra_importrgbaimage_c)
  {
    limo_data *ld_r, *ld_g, *ld_b, *ld_a, *ld_width, *ld_height, *ld_filename;
    int width, height, res;

    REQUIRE_ARGC("vigra_importrgbaimage_c", 7);
    arglist=CDR(arglist);
    ld_r = eval(CAR(arglist), env);        arglist=CDR(arglist);
    ld_g = eval(CAR(arglist), env);        arglist=CDR(arglist);
    ld_b = eval(CAR(arglist), env);        arglist=CDR(arglist);
    ld_a = eval(CAR(arglist), env);        arglist=CDR(arglist);
    ld_width = eval(CAR(arglist), env);    arglist=CDR(arglist);
    ld_height = eval(CAR(arglist), env);   arglist=CDR(arglist);
    ld_filename = eval(CAR(arglist), env); arglist=CDR(arglist);

    width = GETINTFROMMPQ(ld_width);
    height = GETINTFROMMPQ(ld_height);

    res = vigra_importrgbaimage_c((float *)GETCVVECTOR(ld_r), (float *)GETCVVECTOR(ld_g),
				  (float *)GETCVVECTOR(ld_b), (float *)GETCVVECTOR(ld_a),
				  width, height,
				  ld_filename->data.d_string);

    return make_nil(); //make_number_from_long_long(res);
  }
  
  void limo_dll_init(limo_data *env) 
  {
    cvector_type_sym = make_sym("CVECTOR");
    limo_true = make_sym(":T");
    INSBUILTIN(builtin_vigra_imagewidth_c, "VIGRA-IMAGEWIDTH-C");
    INSBUILTIN(builtin_vigra_imageheight_c, "VIGRA-IMAGEHEIGHT-C");
    INSBUILTIN(builtin_vigra_imagenumbands_c, "VIGRA-IMAGENUMBANDS-C");
    INSBUILTIN(builtin_vigra_imagenumextrabands_c, "VIGRA-IMAGENUMEXTRABANDS-C");
    INSBUILTIN(builtin_vigra_importrgbaimage_c, "VIGRA-IMPORTRGBAIMAGE-C");
  }
}
