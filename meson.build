project('klib', 'c', 'cpp',
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
install_subdir('include/klib', install_dir: 'include')

# pkg-config
pkg = import('pkgconfig')
pkg.generate(
    name: 'klib',
    description: 'A generic, type-safe C containers library',
    version: meson.project_version())
klib_dep = declare_dependency(include_directories: include)

# Tests.
tests = [
    ['khash', ['test/khash_test.c']],
    ['khash_keith', ['test/khash_keith.c']],
    ['khash_keith2', ['test/khash_keith2.c']],
    ['kvec', ['test/kvec_test.cc']]
]

foreach t : tests
    exe = executable(
        t.get(0),
        t.get(1),
        include_directories: include)
    test(t.get(0), exe)
endforeach
