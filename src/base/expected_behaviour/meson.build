expected_behaviour_src = [
  'JustLockPtr.cpp',
]

headers_to_install += files(
  'CycleGuard.h',
  'CycleGuard.hpp',
  'SelfShared.h',
  'SelfShared.hpp',
  'JustLockPtr.h',
  'JustLockPtr.hpp',
  'SharedPtr.h',
  'SharedPtr.hpp',
  'SharedPtrWrap.h',
)

expected_behaviour_lib = library(
  'expected_behaviour',
  expected_behaviour_src,
  install: true,
  include_directories: inc,
  implicit_include_directories: false,
)

libs_upto_now += [expected_behaviour_lib]
