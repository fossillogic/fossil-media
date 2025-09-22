dir = include_directories('.')
cc = meson.get_compiler('c')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'cpp')

# Check if the host system is Windows
if host_machine.system() == 'windows'
    winsock_dep = cc.find_library('ws2_32', required: true)
else
    winsock_dep = []
endif

fossil_media_lib = library('fossil_media',
    files('media.c', 'markdown.c', 'yaml.c', 'html.c', 'json.c', 'fson.c', 'text.c', 'toml.c', 'xml.c', 'ini.c', 'csv.c'),
    install: true,
    dependencies: [cc.find_library('m', required: false), winsock_dep],
    include_directories: dir)

fossil_media_dep = declare_dependency(
    link_with: [fossil_media_lib],
    include_directories: dir)

meson.override_dependency('fossil-media', fossil_media_dep)