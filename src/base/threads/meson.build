threads_dep = [dependency('threads')]
threads_src = files(
  'utils.cpp',
)

headers_to_install += files(
  'utils.h',
)

subdir('locks')
subdir('safe_structs')
subdir('message_queue')
subdir('dedicated_thread_scope')


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

subdir('python_bindings')
#subdir('tests')
