document_tree_src = [
  'Container.cpp',
  'DocumentTree.cpp',
  'exceptions.cpp',
]

headers_to_install += files(
  'Container.h',
  'DocumentTree.h',
  'exceptions.h',
)

document_tree_lib = library(
  'document_tree',
  document_tree_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
  dependencies: deps,
)

libs_upto_now += [document_tree_lib]

subdir('python_bindings')
subdir('tests')
