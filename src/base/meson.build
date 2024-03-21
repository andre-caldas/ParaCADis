base_src = [
  'exceptions.cpp',
]

base_includes = [
  'exceptions.h',
]

small_base_lib = library(
  'base',
  base_src,
  include_directories: inc,
  install: true,
)

libs_upto_now = [small_base_lib]

subdir('xml')
subdir('type_traits')
subdir('expected_behaviour')
subdir('threads')
subdir('naming_scheme')
subdir('geometric_primitives')
subdir('document_tree')

base_lib = library(
  'all_base',
  [],
  include_directories: inc,
  install: true,
  link_with: libs_upto_now,
)
