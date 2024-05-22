base_src = [
  'exceptions.cpp',
]

base_includes = [
  'exceptions.h',
]

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
subdir('geometric_primitives')
subdir('mesh_provider')
subdir('document_tree')
