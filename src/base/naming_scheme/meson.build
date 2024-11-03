naming_scheme_src = [
  'exceptions.cpp',
  'Exporter.cpp',
  'NameAndUuid.cpp',
  'PathToken.cpp',
  'PathToObject.cpp',
  'Uuid.cpp',
]

naming_scheme_includes = [
  'exceptions.h',
  'path_cache_policies.h',
  'path_cache_policies.hpp',
  'Exporter.h',
  'IExport.h',
  'IExport.hpp',
  'NameAndUuid.h',
  'NameSearch.h',
  'NameSearch.hpp',
  'PathToken.h',
  'PathToObject.h',
  'Uuid.h',
  'ReferenceToObject.h',
  'ReferenceToObject.hpp',
  'types.h',
]

install_headers(naming_scheme_includes,
  preserve_path: true,
)

naming_scheme_lib = library(
  'naming_scheme',
  naming_scheme_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
)

libs_upto_now += [naming_scheme_lib]

subdir('python_bindings')
