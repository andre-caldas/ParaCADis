subdir('type_traits')
subdir('document_tree')
subdir('expected_behaviour')
subdir('geometric_primitives')
subdir('naming_scheme')
subdir('threads')

base_src = [
  'exceptions.cpp',
]

base_includes = [
  'exceptions.h',
]

base_lib = shared_library(
  'base',
  base_src,
  include_directories: inc,
  install: true,
)
