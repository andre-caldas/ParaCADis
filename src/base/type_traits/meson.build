type_traits_src = [
]

type_traits_includes = [
  'IndexTraits.h',
  'IsIterator.h',
  'MultiIndexRecordInfo.h',
  'record_info.h',
  'record_items.h',
  'ReduceToRaw.h',
  'Utils.h',
]

install_headers(type_traits_includes,
  preserve_path: true,
)

type_traits_lib = shared_library(
  'type_traits',
  type_traits_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)
