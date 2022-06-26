add_library (file file.c)

# include-directory for this library
target_include_directories (file PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../gc-8.0.6/include                # libgc
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../gmp/${CMAKE_ANDROID_ARCH_ABI}   # libgmp
  )
