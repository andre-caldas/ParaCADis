xml_src = [
]

xml_includes = [
  'streams.h',
]

install_headers(xml_includes,
  preserve_path: true,
)

xml_lib = library(
  'xml',
  xml_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)
