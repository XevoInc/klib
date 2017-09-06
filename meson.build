project('klib', 'c',
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
include = include_directories('.')
install_subdir('include/klib', install_dir: 'include')

# pkg-config
pkg = import('pkgconfig')
pkg.generate(
    name: 'klib',
    description: 'A generic, type-safe C containers library',
    version: meson.project_version())
klib_dep = declare_dependency(include_directories: include)
