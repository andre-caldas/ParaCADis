imgui_src = files(
  'ImGuiScope.cpp',
)

headers_to_install += files(
  'ImGuiDialog.h',
  'ImGuiDialog.hpp',
  'ImGuiElement.h',
  'ImGuiElement.hpp',
  'ImGuiScope.h',
  'ImGuiScope.hpp',
)

imgui_deps = [
  dependency('OGRE'),
  dependency('OGRE-Bites'),
  dependency('OGRE-Overlay'),
]

imgui_lib = library(
  'paracadis_imgui',
  imgui_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: true,
  link_with: [libs_base],
  dependencies: [imgui_deps],
)

subdir('python_bindings')
#subdir('tests')
