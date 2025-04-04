pybind_imgui_src = files()

subdir('imgui')

paracadis_misc_py_lib = py_ins.extension_module(
  'paracadis_misc',
  [pybind_imgui_src],
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: [deps, py_deps],
  link_with: [libs_base, imgui_lib],
)
