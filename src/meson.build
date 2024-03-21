inc = include_directories('.')

deps = [
  dependency('gmpxx'),
  dependency('mpfr'),
]

test_deps = [
  dependency('catch2'),
]

test_main_src = files('cpp_test.cpp')

subdir('base')

paracadis_exec = executable('paracadis', 'paracadis.cpp',
  include_directories: inc,
  dependencies: deps,
)
