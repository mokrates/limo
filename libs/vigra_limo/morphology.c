///// preamble
#include <vigra_c.h>
#include <vigra_morphology_c.h>

extern "C" {
#include <limo.h>
#include "vigra_limo.h"
#include "../cvector/cvector.h"
  
  //////// End of Preamble

  BUILTIN(builtin_vigra_upwindimage_c) {
    limo_data *arr_in, *arr_fac_in, *arr_out,
      *ld_width, *ld_height, *ld_weight;
    int width, height;
    float weight;

    REQUIRE_ARGC("vigra_upwindimage_c", 6);
    arglist=CDR(arglist);
    arr_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_fac_in = eval(CAR(arglist), env); arglist=CDR(arglist);
    arr_out = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_width = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_height = eval(CAR(arglist), env); arglist=CDR(arglist);
    ld_weight = eval(CAR(arglist), env);

    width = GETINTFROMMPQ(ld_width);
    height = GETINTFROMMPQ(ld_height);
    weight = GETDOUBLEFROMMPQ(ld_weight);

    if (!vigra_upwindimage_c((float *)GETCVVECTOR(arr_in), (float *)GETCVVECTOR(arr_fac_in),
				 (float *)GETCVVECTOR(arr_out),
				 width, height, weight))
      return sym_true;
    else
      return nil;
  }
}
