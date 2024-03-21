locks_dep = dependency('threads')

locks_src = [
  'exceptions.cpp',
  'LockPolicy.cpp',
  'YesItIsAMutex.cpp',
]

locks_includes = [
  'exceptions.h',
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

locks_lib = library(
  'locks',
  locks_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: locks_dep,
  link_with: libs_upto_now,
)

libs_upto_now += [locks_lib]
