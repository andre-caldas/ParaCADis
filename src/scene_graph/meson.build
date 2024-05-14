scene_graph_src = [
  'ContainerNode.cpp',
#  'NurbsNode.cpp',
  'SceneRoot.cpp',
]

scene_graph_includes = [
  'ContainerNode.h',
  'NurbsNode.h',
  'SceneRoot.h',
  'UiState.h',
]

install_headers(
  scene_graph_includes,
  preserve_path: true,
)

scene_graph_deps = [
  dependency('gismo'),
  dependency('OGRE'),
  dependency('vulkan'),
]

scene_graph_lib = library(
  'scene_graph',
  scene_graph_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
  dependencies: [deps, scene_graph_deps],
)

libs_upto_now += [scene_graph_lib]

#subdir('python_bindings')
#subdir('tests')
