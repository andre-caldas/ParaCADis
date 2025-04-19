pybind_imgui_src = files(
  'module.cpp',
  'imgui_scope.cpp',
)

imgui_py_lib = py_ins.extension_module(
  'paracadis_imgui',
  pybind_imgui_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: [deps, py_deps, imgui_deps],
  link_with: [libs_upto_now, imgui_lib],
)
