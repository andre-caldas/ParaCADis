scene_graph_src = [
  'ContainerNode.cpp',
#  'MeshNode.cpp',
  'SceneRoot.cpp',
]

scene_graph_includes = [
  'ContainerNode.h',
  'MeshNode.h',
  'SceneRoot.h',
  'UiState.h',
]

install_headers(
  scene_graph_includes,
  preserve_path: true,
)

ogre_dep = dependency('OGRE')
scene_graph_deps = [
  ogre_dep,
]

scene_graph_conf = configuration_data()
scene_graph_conf.set('OGRE_DEFAULT_CONFIG_DIR',
                     '"' + ogre_dep.get_variable('prefix')
                     + '/share/OGRE/"',
                     description: 'Directory for plataform independent files')

scene_graph_conf.set('OGRE_DEFAULT_PLUGIN_CFG',
                     'OGRE_DEFAULT_CONFIG_DIR "plugins" OGRE_BUILD_SUFFIX ".cfg"',
                     description: 'Available plugins data')

configure_file(output : 'config.h',
               configuration : scene_graph_conf)

scene_graph_lib = library(
  'scene_graph',
  scene_graph_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: true,
  link_with: libs_base,
  dependencies: [deps, scene_graph_deps],
)

libs_upto_now += [scene_graph_lib]

subdir('python_bindings')
#subdir('tests')
