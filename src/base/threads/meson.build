threads_dep = [dependency('threads')]
threads_src = files(
  'utils.cpp',
)
threads_includes = files(
  'utils.h',
)

subdir('locks')
subdir('safe_structs')
subdir('message_queue')
subdir('dedicated_thread_scope')


install_headers(threads_includes,
  preserve_path: true,
)

threads_lib = library(
  'threads',
  threads_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: threads_dep,
  link_with: libs_upto_now,
)

libs_upto_now += [threads_lib]
