expected_behaviour_src = [
  'SharedPtr.cpp'
]

expected_behaviour_includes = [
  'SharedPtr.h',
  'SharedPtr_impl.h',
]

install_headers(expected_behaviour_includes,
  preserve_path: true,
)

expected_behaviour_lib = library(
  'expected_behaviour',
  expected_behaviour_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)

libs_upto_now += [expected_behaviour_lib]
