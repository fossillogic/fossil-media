dir = include_directories('.')
cc = meson.get_compiler('c')

# Check if the host system is Windows
if host_machine.system() == 'windows'
    winsock_dep = cc.find_library('ws2_32', required: true)
else
    winsock_dep = []
endif

fossil_media_lib = library('fossil_media',
    files('media.c', 'markdown.c', 'yaml.c', 'json.c', 'text.c', 'toml.c', 'xml.c', 'elf.c', 'ini.c', 'csv.c'),
    install: true,
    dependencies: [cc.find_library('m', required: false), winsock_dep],
    include_directories: dir)

fossil_media_dep = declare_dependency(
    link_with: [fossil_media_lib],
    include_directories: dir)

meson.override_dependency('fossil-media', fossil_media_dep)