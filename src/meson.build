inc = include_directories('.')

add_project_arguments('-DCGAL_HAS_THREADS', language : 'cpp')

deps = [
  dependency('CGAL'),
  dependency('gismo'),
  dependency('openmp'),
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
  dependency('pybind11'),
]

test_main_src = files('cpp_test.cpp')

pybind_main_src = files()

headers_to_install = files()

subdir('base')
subdir('mesh_provider')
subdir('scene_graph')
subdir('misc')
subdir('python_bindings')

paracadis_exec = executable('paracadis', 'paracadis.cpp',
  include_directories: inc,
  dependencies: deps,
)

fs = import('fs')

# https://github.com/mesonbuild/meson/issues/13094
headers_to_install_with_paths = []
foreach f : headers_to_install
  headers_to_install_with_paths += fs.relative_to(f, '')
endforeach

install_headers(
  headers_to_install_with_paths,
  subdir: meson.project_name(),
  preserve_path: true,
)
