project('slowjson', 'c', meson_version: '>=1.3.0', default_options: [
  'c_std=c11', 'warning_level=3', 'buildtype=debug', 'b_ndebug=if-release',
])

cc = meson.get_compiler('c')
if get_option('debug')
  add_project_arguments('-DDEBUG', language: 'c')
endif
if get_option('buildtype') == 'release'
  add_project_arguments(cc.get_supported_arguments('-march=native'), language: 'c')
endif
if get_option('b_sanitize') != 'none'
  add_project_arguments(cc.get_supported_arguments(
    '-fno-omit-frame-pointer', '-fno-sanitize-recover=all'), language: 'c')
endif

json_sources = files(
  'src/json/internal/config.c',
  'src/json/internal/document.c',
  'src/json/internal/jsonvalue.c',
  'src/json/internal/lexer/main.c',
  'src/json/internal/lexer/number.c',
  'src/json/internal/parser.c',
  'src/json/internal/serialize.c',
  'src/json/utils/alloc/arena.c',
  'src/json/utils/custom_error.c',
  'src/json/utils/hashmap/hashmap.c',
  'src/json/utils/hashmap/hashmap_node.c',
  'src/json/utils/list.c',
  'src/json/utils/string/dstring.c',
  'src/json/utils/string/file.c',
  'src/json/utils/string/string_view.c',
  'src/json/utils/unicode/utf16.c',
  'src/json/utils/unicode/utf8.c',
  'src/json/utils/unicode/wchar.c',
)
json_lib = static_library('slowjson', json_sources)
json_dep = declare_dependency(link_with: json_lib, include_directories: include_directories('src'))

gradeviewer = executable('gradeviewer', files(
  'src/main/main.c',
  'src/data/load.c',
  'src/data/save.c',
  'src/midend/data.c',
  'src/midend/mid.c',
  'src/frontend/edit_events/edit_events.c',
  'src/frontend/text_formatting/text_formatting.c',
  'src/frontend/user_input/user_input.c',
), dependencies: json_dep)

test_runner = executable('gradeviewer_tests', files(
  'tools/test/test.c',
  'tools/test/jsontestsuite.c',
  'tools/test/fxx_test.c',
  'tools/test/jsonperf.c',
), dependencies: json_dep, build_by_default: false)
test('slowjson', test_runner, workdir: meson.project_source_root(), timeout: 0)

executable('slowjson-fuzz', 'tools/fuzz/fuzz.c',
  dependencies: [json_dep, dependency('threads')], build_by_default: false)
run_target('run', command: gradeviewer)

# These setups wrap the same test executable, including --test-args passthrough.
# ASan and Valgrind must not be used together.
if get_option('b_sanitize') == 'none'
  add_test_setup('valgrind', exe_wrapper: [
    'valgrind', '--leak-check=full', '--show-leak-kinds=all', '--error-exitcode=1',
  ], timeout_multiplier: 0)
  add_test_setup('callgrind', exe_wrapper: [
    'valgrind', '--tool=callgrind', '--dump-instr=yes',
    '--callgrind-out-file=' + meson.current_build_dir() / 'callgrind.out.%p',
  ], timeout_multiplier: 0)
endif
