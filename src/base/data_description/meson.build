data_description_src = files(
  'GateTranslator.cpp',
  'Description.cpp',
)

headers_to_install += files(
  'DataTranslator.h',
  'GateTranslator.h',
  'GateTranslator.hpp',
  'Description.h',
  'Description.hpp',
)

data_description_lib = library(
  'data_description',
  data_description_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
  dependencies: deps,
)

libs_upto_now += [data_description_lib]

#subdir('python_bindings')
#subdir('tests')
