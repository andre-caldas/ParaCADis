inc = include_directories('.')

add_project_arguments('-DCGAL_HAS_THREADS', language : 'cpp')

deps = [
  dependency('CGAL'),
  dependency('gmpxx'),
  dependency('mpfr'),
]

test_deps = [
  dependency('catch2'),
  dependency('catch2-with-main'),
]

py_mod = import('python')
py_ins = py_mod.find_installation(pure: false)
py_deps = [
  py_ins.dependency(),
  dependency('nanobind'),
]

test_main_src = files('cpp_test.cpp')

pybind_src = []

subdir('base')
subdir('scene_graph')
subdir('python_bindings')

paracadis_exec = executable('paracadis', 'paracadis.cpp',
  include_directories: inc,
  dependencies: deps,
)
