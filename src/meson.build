inc = include_directories('.')

deps = [
  dependency('gmpxx'),
  dependency('mpfr'),
]

test_deps = [
  dependency('catch2'),
  dependency('catch2-with-main'),
]

test_main_src = files('cpp_test.cpp')

subdir('base')

paracadis_exec = executable('paracadis', 'paracadis.cpp',
  include_directories: inc,
  dependencies: deps,
)
