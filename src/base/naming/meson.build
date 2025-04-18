naming_scheme_src = [
  'exceptions.cpp',
  'Exporter.cpp',
  'NameAndUuid.cpp',
  'PathToken.cpp',
  'PathToObject.cpp',
  'Uuid.cpp',
]

headers_to_install += files(
  'exceptions.h',
  'path_cache_policies.h',
  'path_cache_policies.hpp',
  'Chainable.h',
  'Chainable.hpp',
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
)

naming_scheme_lib = library(
  'naming',
  naming_scheme_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
)

libs_upto_now += [naming_scheme_lib]

subdir('python_bindings')
