document_tree_tests_src = [
  'tests.cpp'
]

test_exec = executable('document_tree_test', [test_main_src, document_tree_tests_src],
  include_directories: inc,
  dependencies: [deps, test_deps],
  link_with: libs_upto_now,
)
test('Tests base/document_tree library objects', test_exec)
