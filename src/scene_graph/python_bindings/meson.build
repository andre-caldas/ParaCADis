pybind_scene_graph_src = files(
  'module.cpp',
  'scene.cpp',
)

paracadis_py_lib = py_ins.extension_module(
  'paracadis_scene_graph',
  pybind_scene_graph_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  dependencies: [deps, py_deps, scene_graph_deps],
  link_with: [libs_base, scene_graph_lib],
)
