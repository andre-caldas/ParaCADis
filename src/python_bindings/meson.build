pybind_main_src += files(
  'module.cpp',
)

paracadis_py_lib = py_ins.extension_module(
  'paracadis',
  pybind_main_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: [deps, py_deps],
  link_with: libs_upto_now,
)
