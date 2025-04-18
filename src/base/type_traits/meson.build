type_traits_src = [
]

headers_to_install += files(
  'IndexTraits.h',
  'IsIterator.h',
  'MultiIndexRecordInfo.h',
  'PowerCast.h',
  'record_info.h',
  'record_items.h',
  'ReduceToRaw.h',
  'Utils.h',
  'NamedMember.h',
)

type_traits_lib = shared_library(
  'type_traits',
  type_traits_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)
