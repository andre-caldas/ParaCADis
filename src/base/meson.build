base_src = files(
  'exceptions.cpp',
)

base_includes = files(
  'exceptions.h',
)

base_lib = library(
  'base',
  base_src,
  include_directories: inc,
  install: true,
)

libs_upto_now = [base_lib]

subdir('xml')
subdir('type_traits')
subdir('expected_behaviour')
subdir('threads')
subdir('naming_scheme')
subdir('data_description')
subdir('geometric_primitives')
subdir('document_tree')

libs_base = libs_upto_now
