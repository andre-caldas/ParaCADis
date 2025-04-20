xml_src = [
  'streams/Reader.cpp',
  'streams/Writer.cpp',
  'streams/XmlTag.cpp',
  'exceptions.cpp',
]

headers_to_install += files(
  'exceptions.h',
  'streams.h',
  'streams_fwd.h',
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
