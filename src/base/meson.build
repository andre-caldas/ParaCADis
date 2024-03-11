subdir('document_tree')
subdir('expected_behaviour')
subdir('geometric_primitives')
subdir('naming_scheme')
subdir('thread_safe_structs')

base_src = [
  'exceptions.cpp',
]

base_lib = shared_library(
  'base',
  base_src,
  link_with: [coordinate_system_lib],
  install: true,
)
