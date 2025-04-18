geometric_primitives_src = files(
  'checks.cpp',
  'deferenceables.cpp',
  'deferenceables_translation.cpp',
  'DocumentGeometry.cpp',
  'exceptions.cpp',
  'types.cpp',
  'circles.cpp',
  'circles_translation.cpp',
  'lines.cpp',
  'lines_translation.cpp',
  'spheres.cpp',
  'spheres_translation.cpp',
)

headers_to_install += files(
  'all_translators.h',
  'checks.h',
  'deferenceables.h',
  'deferenceables_description.h',
  'deferenceables_translation.h',
  'DocumentGeometry.h',
  'exceptions.h',
  'types.h',
  'circles.h',
  'circles_description.h',
  'circles_translation.h',
  'lines.h',
  'lines_description.h',
  'lines_translation.h',
  'spheres.h',
  'spheres_description.h',
  'spheres_translation.h',
  'power_cast.h',
)

subdir('coordinate_system')

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
