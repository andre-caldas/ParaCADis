safe_containers_dep = dependency('threads')

safe_containers_src = [
]

safe_containers_includes = [
  'common_records/WeakPtrRecord.h',
  'MultiIndexContainer.h',
  'MultiIndexContainer_inl.h',
  'ThreadSafeContainer.h',
  'ThreadSafeContainer_inl.h',
  'ThreadSafeMap.h',
  'ThreadSafeMultiIndex.h',
  'ThreadSafeMultiIndex_inl.h',
  'ThreadSafeStruct.h',
  'ThreadSafeStruct_inl.h',
  'ThreadSafeVector.h',
]

install_headers(safe_containers_includes,
  preserve_path: true,
)

safe_containers_lib = library(
  'safe_containers',
  safe_containers_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: safe_containers_dep,
)
