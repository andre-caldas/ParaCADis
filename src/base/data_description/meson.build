data_description_src = files(
  'DataTranslator.cpp',
  'GateTranslator.cpp',
  'DataWithDescription.cpp',
)

data_description_includes = files(
  'DataTranslator.h',
  'GateTranslator.h',
  'DataWithDescription.h',
  'DataWithDescription.hpp',
)

install_headers(data_description_includes,
  preserve_path: true,
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
