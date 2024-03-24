xml_src = [
  'streams/Reader.cpp',
  'streams/Writer.cpp',
  'streams/XmlTag.cpp',
  'exceptions.cpp',
]

xml_includes = [
  'exceptions.h',
  'streams.h',
  'streams_fwd.h',
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
  link_with: base_lib,
)

libs_upto_now += [xml_lib]
