subdir('coordinate_system')
subdir('expected_behaviour')
subdir('naming_scheme')
subdir('thread_safe_structs')

base_src = [
  'Container.cpp',
  'DocumentTree.cpp',
  'TreeNode.cpp',
]

base_lib = shared_library(
  'base',
  base_src,
  link_with: [coordinate_system_lib],
  install: true,
)
