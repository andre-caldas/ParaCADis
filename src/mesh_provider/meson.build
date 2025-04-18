mesh_provider_src = [
  'GlThreadQueue.cpp',
  'IgaProvider.cpp',
  'MeshProvider.cpp',
  'OgreGismoMesh.cpp',
]

mesh_provider_includes = [
  'GlThreadQueue.h',
  'IgaProvider.h',
  'IgaProvider.hpp',
  'MeshProvider.h',
  'MeshProvider.hpp',
  'OgreGismoMesh.h',
]

install_headers(mesh_provider_includes,
  preserve_path: true,
)

mesh_provider_deps = [
  dependency('gismo'),
  dependency('OGRE'),
]

mesh_provider_lib = library(
  'mesh_provider',
  mesh_provider_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_base,
  dependencies: [deps, mesh_provider_deps],
)
