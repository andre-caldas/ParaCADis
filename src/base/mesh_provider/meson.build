mesh_provider_src = [
  'mesh_circles.cpp',
  'mesh_lines.cpp',
]

mesh_provider_includes = [
  'LineMesh.h',
  'MeshProvider.h',
  'MeshProvider_impl.h',
  'mesh_circles.h',
  'mesh_lines.h',
]

install_headers(mesh_provider_includes,
  preserve_path: true,
)

mesh_provider_lib = library(
  'mesh_provider',
  mesh_provider_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
  link_with: libs_upto_now,
  dependencies: deps,
)

libs_upto_now += [mesh_provider_lib]
