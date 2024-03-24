document_tree_src = [
  'Container.cpp',
  'DocumentTree.cpp',
  'exceptions.cpp',
]

document_tree_includes = [
  'Container.h',
  'DocumentTree.h',
  'exceptions.h',
]

install_headers(
  document_tree_includes,
  preserve_path: true,
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

subdir('tests')
