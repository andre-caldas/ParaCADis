mesh_provider_src = [
  'GlThreadQueue.cpp',
  'IgaProvider.cpp',
  'MeshProvider.cpp',
  'OgreGismoMesh.cpp',
]

headers_to_install += files(
  'GlThreadQueue.h',
  'IgaProvider.h',
  'MeshProvider.h',
  'OgreGismoMesh.h',
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
