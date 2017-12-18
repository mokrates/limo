///// preamble
#include <vigra_c.h>
#include <vigra_tensors_c.h>

extern "C" {
#include <limo.h>
#include "vigra_limo.h"
#include "../cvector/cvector.h"
  
  //////// End of Preamble

  
  BUILTIN(builtin_vigra_structuretensor_c) {
    limo_data *arr_in, *arr_xx_out, *arr_xy_out, *arr_yy_out,
      *ld_width, *ld_height, *ld_inner_scale, *ld_outer_scale;
    int width, height;
    float inner_scale, outer_scale;

    REQUIRE_ARGC("vigra_structuretensor_c", 8);
    arglist=CDR(arglist);
    arr_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_xx_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_xy_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_yy_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_width = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_height = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_inner_scale = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_outer_scale = eval(CAR(arglist), env);

    width = GETINTFROMMPQ(ld_width);
    height = GETINTFROMMPQ(ld_height);
    inner_scale = GETDOUBLEFROMMPQ(ld_inner_scale);
    outer_scale = GETDOUBLEFROMMPQ(ld_outer_scale);

    if (!vigra_structuretensor_c((float *)GETCVVECTOR(arr_in), (float *)GETCVVECTOR(arr_xx_out),
				 (float *)GETCVVECTOR(arr_xy_out), (float *)GETCVVECTOR(arr_yy_out),
				 width, height, inner_scale, outer_scale))
      return sym_true;
    else
      return nil;
  }

  BUILTIN(builtin_vigra_tensoreigenrepresentation_c) {
    limo_data *arr_xx_in, *arr_xy_in, *arr_yy_in,
      *arr_lEV_out, *arr_sEV_out, *arr_ang_out,
      *ld_width, *ld_height;
    int width, height;
    
    REQUIRE_ARGC("vigra_tensoreigenrepresentation_c", 8);
    arglist=CDR(arglist);
    arr_xx_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_xy_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_yy_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_lEV_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_sEV_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_ang_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_width = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_height= eval(CAR(arglist), env);

    width = GETINTFROMMPQ(ld_width);
    height= GETINTFROMMPQ(ld_height);

    if (!vigra_tensoreigenrepresentation_c((float *)GETCVVECTOR(arr_xx_in), (float *)GETCVVECTOR(arr_xy_in),
					   (float *)GETCVVECTOR(arr_yy_in), (float *)GETCVVECTOR(arr_lEV_out),
					   (float *)GETCVVECTOR(arr_sEV_out), (float *)GETCVVECTOR(arr_ang_out),
					   width, height))
      return sym_true;
    else
      return nil;
  }
}
