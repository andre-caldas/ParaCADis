mesh_provider_src = [
]

mesh_provider_includes = [
  'MeshProvider.h',
  'MeshProviderConcepts.h',
  'MeshProvider_impl.h',
]

install_headers(mesh_provider_includes,
  preserve_path: true,
)

mesh_provider_deps = [
  dependency('gismo'),
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

libs_upto_now += [mesh_provider_lib]
