project('xlib', 'c', 'cpp',
    version: '0.1',
    license: 'mit',
    default_options: [
        'c_std=c11',
        'buildtype=debug',
        'warning_level=3',
        'werror=true'
    ],
)

cflags = [
    '-Wno-unused-function',
    '-Wno-unused-parameter',
]
add_project_arguments(cflags, language: 'c')
add_project_arguments(cflags, language: 'cpp')

# Includes.
include = include_directories('include','.')
install_subdir('include/xlib', install_dir: 'include')

src = [
    'src/xargparse.c'
]

lib = static_library(
    'xlib',
    src,
    include_directories: include,
    install: true,
    version: meson.project_version())

# pkg-config
pkg = import('pkgconfig')
pkg.generate(
    name: 'xlib',
    description: 'A generic, type-safe C containers library',
    version: meson.project_version())
xlib_dep = declare_dependency(include_directories: include)

# unit tests FIXME - compile all C files in test directory
test_deps = [
    dependency('valgrind')
]
executable(
    'test_cmd_opt',
    ['test/test_cmd_opt.c'],
    include_directories: include,
    dependencies: test_deps
)

# Convenient targets.
run_target(
    'run-test_cmd_opt',
    command: join_paths(meson.current_build_dir(), 'test_cmd_opt'))

