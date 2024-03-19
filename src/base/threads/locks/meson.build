locks_dep = dependency('threads')

locks_src = [
  'LockPolicy.cpp',
  'YesItIsAMutex.cpp',
]

locks_includes = [
  'LockedIterator.h',
  'LockPolicy.h',
  'LockPolicy_inl.h',
  'MutexData.h',
  'ReaderLock.h',
  'ReaderLock_inl.h',
  'WriterLock.h',
  'WriterLock_inl.h',
  'YesItIsAMutex.h',
]

install_headers(locks_includes,
  preserve_path: true,
)

locks_lib = shared_library(
  'locks',
  locks_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: locks_dep,
)

libs_upto_now += [locks_lib]
