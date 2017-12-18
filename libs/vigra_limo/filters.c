///// preamble
#include <vigra_c.h>
#include <vigra_filters_c.h>

extern "C" {
#include <limo.h>
#include "vigra_limo.h"
#include "../cvector/cvector.h"
  
  //////// End of Preamble

  BUILTIN(builtin_vigra_hessianmatrixofgaussian_c) {
    limo_data *arr_in, *arr_xx_out, *arr_xy_out, *arr_yy_out,
      *ld_width, *ld_height, *ld_scale;
    int width, height;
    float scale;

    REQUIRE_ARGC("vigra_hessianmatrixofgaussian_c", 7);
    arglist=CDR(arglist);
    arr_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_xx_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_xy_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_yy_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_width = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_height = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_scale = eval(CAR(arglist), env);

    width = GETINTFROMMPQ(ld_width);
    height = GETINTFROMMPQ(ld_height);
    scale = GETDOUBLEFROMMPQ(ld_scale);

    if (!vigra_hessianmatrixofgaussian_c((float *)GETCVVECTOR(arr_in), (float *)GETCVVECTOR(arr_xx_out),
				 (float *)GETCVVECTOR(arr_xy_out), (float *)GETCVVECTOR(arr_yy_out),
				 width, height, scale))
      return sym_true;
    else
      return nil;
  }


}
