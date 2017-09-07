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

# Includes.
include = include_directories('include')
install_subdir('include/xlib', install_dir: 'include')

# pkg-config
pkg = import('pkgconfig')
pkg.generate(
    name: 'xlib',
    description: 'A generic, type-safe C containers library',
    version: meson.project_version())
xlib_dep = declare_dependency(include_directories: include)

# Tests.
tests = [
    ['xhash', ['test/xhash_test.c']],
    ['xhash_keith', ['test/xhash_keith.c']],
    ['xhash_keith2', ['test/xhash_keith2.c']],
    ['xvec', ['test/xvec_test.cpp']]
]

foreach t : tests
    exe = executable(
        t.get(0),
        t.get(1),
        include_directories: include)
    test(t.get(0), exe)
endforeach
