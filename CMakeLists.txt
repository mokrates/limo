# CMakeLists.txt for building liblimo.a for android

set(CMAKE_VERBOSE_MAKEFILE ON)

add_library (limo
  annotations.c
  block.c
  builtinenv.c
  builtins.c
  dict.c
  dynamicvars.c
  error.c
  eval.c
  helpers.c
  inlined_mods.c
  limo.c
  limpy.c
  makers.c
  misc.c
  numbers_gmp.c
  reader.c
  special.c
  vars.c
  writer.c)

add_subdirectory (libs/string_builtins)
target_link_libraries (limo string_builtins)

target_include_directories (limo SYSTEM PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue ../gc-8.0.6/include ../gmp/${CMAKE_ANDROID_ARCH_ABI})
