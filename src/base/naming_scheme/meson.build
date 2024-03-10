boost_dep = dependencies('boost', modules: 'uuid')

naming_scheme_src = [
  'IExport.cpp.cpp',
  'NameAndUuid.cpp',
  'PathToObject.cpp',
  'ReferenceToObject.cpp',
  'ReferencedObject.cpp',
  'exceptions.cpp',
]

naming_scheme_lib = shared_library(
  'naming_scheme',
  naming_scheme_src,
  dependencies: boost_dep,
  install: true,
)
