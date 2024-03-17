naming_scheme_src = [
  'exceptions.cpp',
  'Exporter.cpp',
  'NameAndUuid.cpp',
  'NameSearchResult.cpp',
  'PathToObject.cpp',
]

naming_scheme_includes = [
  'exceptions.h',
  'Exporter.h',
  'IExport.h',
  'IExport_impl.h',
  'NameAndUuid.h',
  'NameSearchResult.h',
  'NameSearchResult_impl.h',
  'PathToObject.h',
  'ReferenceToObject.h',
  'ReferenceToObject_impl.h',
  'types.h',
]

install_headers(naming_scheme_includes,
  preserve_path: true,
)

naming_scheme_lib = shared_library(
  'naming_scheme',
  naming_scheme_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)
