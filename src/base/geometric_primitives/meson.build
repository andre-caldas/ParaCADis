geometric_primitives_src = [
  'checks.cpp',
  'circles.cpp',
  'CoordinateSystem.cpp',
  'deferenceables.cpp',
  'exceptions.cpp',
  'lines.cpp',
  'spheres.cpp',
  'types.cpp',
]

geometric_primitives_includes = [
  'checks.h',
  'circles.h',
  'CoordinateSystem.h',
  'deferenceables.h',
  'exceptions.h',
  'lines.h',
  'spheres.h',
  'types.h',
]

install_headers(geometric_primitives_includes,
  preserve_path: true,
)

geometric_primitives_lib = library(
  'geometric_primitives',
  geometric_primitives_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
  dependencies: deps,
)

libs_upto_now += [geometric_primitives_lib]

subdir('python_bindings')
