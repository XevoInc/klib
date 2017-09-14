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

add_project_arguments([], language: 'c')

# Includes.
include = include_directories('include')
install_subdir('include/xlib', install_dir: 'include')

src = [
    'src/xargparse.c'
]

lib = library(
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
    libraries: [lib],
    version: meson.project_version())
xlib_dep = declare_dependency(include_directories: include)

# unit tests FIXME - compile all C files in test directory

executable(
    'test_argparse',
    ['test/test_argparse.c'],
    link_with: lib,
    include_directories: include
)

# Convenient targets.
run_target(
    'run-test_cmd_opt',
    command: join_paths(meson.current_build_dir(), 'test_cmd_opt'))

