threads_dep = dependency('threads')

threads_src = [
  'locks/LockPolicy.cpp',
]

threads_lib = shared_library(
  'threads',
  threads_src,
  dependencies: [thread_dep],
  install: true,
)
