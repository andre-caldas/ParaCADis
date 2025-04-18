scene_graph_src = [
  'ContainerNode.cpp',
  'MeshNode.cpp',
  'RenderingScope.cpp',
  'SceneRoot.cpp',
]

headers_to_install += files(
  'ContainerNode.h',
  'MeshNode.h',
  'RenderingScope.h',
  'SceneRoot.h',
)

scene_graph_deps = [
  dependency('OGRE'),
  dependency('OGRE-Bites'),
  dependency('OGRE-RTShaderSystem'),
]

scene_graph_lib = library(
  'scene_graph',
  scene_graph_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: true,
  link_with: [libs_base, mesh_provider_lib],
  dependencies: [deps, scene_graph_deps],
)

libs_upto_now += [scene_graph_lib]

subdir('python_bindings')
#subdir('tests')
