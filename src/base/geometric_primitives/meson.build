geometric_primitives_src = [
  'checks.cpp',
  'circles.cpp',
  'CoordinateSystem.cpp',
  'deferenceables.cpp',
  'GeometricObject.cpp',
  'lines.cpp',
]

geometric_primitives_includes = [
  'checks.h',
  'circles.h',
  'CoordinateSystem.h',
  'deferenceables.h',
  'exceptions.h',
  'GeometricObject.h',
  'lines.h',
  'types.h',
]

install_headers(geometric_primitives_includes,
  preserve_path: true,
)

geometric_primitives_lib = shared_library(
  'geometric_primitives',
  geometric_primitives_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)

libs_upto_now += [geometric_primitives_lib]
