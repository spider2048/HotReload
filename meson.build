project('HotReloading', 'cpp')

cc = meson.get_compiler('cpp')
sources = [
    'src/main.cpp'
]

shared_library('HotReload', 
                sources,
                include_directories: include_directories('./include'),
                cpp_args: ['/std:c++20'],
                dependencies: dependency('fmt'))