inc = include_directories('.')

all_libraries = []

deps = [
  dependency('gmpxx'),
  dependency('mpfr'),
]

test_deps = [
  dependency('catch2'),
]

subdir('base')

paracadis_exec = executable('paracadis', 'paracadis.cpp',
  include_directories: inc,
  dependencies: deps,
)

test_exec = executable('cpp_test', 'cpp_test.cpp',
  include_directories: inc,
  dependencies: [deps, test_deps]
)
test('Tests all cpp library objects', test_exec)
