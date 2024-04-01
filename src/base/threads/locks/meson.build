locks_dep = dependency('threads')

locks_src = [
  'exceptions.cpp',
  'LockPolicy.cpp',
  'reader_locks.cpp',
  'writer_locks.cpp',
  'YesItIsAMutex.cpp',
]

locks_includes = [
  'exceptions.h',
  'gates.h',
  'gates_impl.h',
  'LockedIterator.h',
  'LockPolicy.h',
  'LockPolicy_inl.h',
  'meson.build',
  'MutexData.h',
  'PolicyProxy.h',
  'PolicyProxy_inl.h',
  'reader_locks.h',
  'writer_locks.h',
  'writer_locks_impl.h',
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
