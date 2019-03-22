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

src = []
if get_option('xlog')
    src += 'src/xlog.c'
    install_headers('include/xlib/xlog.h', subdir: 'xlib')
endif
if get_option('xargparse')
    src += 'src/xargparse.c'
    install_headers('include/xlib/xargparse.h', subdir: 'xlib')
endif

install_headers('include/xlib/xassert.h', subdir: 'xlib')
install_headers('include/xlib/xhash.h', subdir: 'xlib')
install_headers('include/xlib/xvec.h', subdir: 'xlib')

# If we have no sources, we will do a header-only install.
if src.length() > 0
    lib = library(
        'xlib',
        src,
        include_directories: include,
        install: true,
        version: meson.project_version())
endif

# pkg-config
pkg = import('pkgconfig')
pkg.generate(
    name: 'xlib',
    description: 'A generic, type-safe C containers library',
    libraries: [lib],
    version: meson.project_version())
xlib_dep = declare_dependency(link_with: lib, include_directories: include)

if get_option('xargparse') and get_option('tests')
    executable(
        'test_argparse',
        ['test/test-argparse.c'],
        link_with: lib,
        include_directories: include
    )
endif
