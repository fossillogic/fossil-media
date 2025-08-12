dir = include_directories('.')
cc = meson.get_compiler('c')

# Check if the host system is Windows
if host_machine.system() == 'windows'
    winsock_dep = cc.find_library('ws2_32', required: true)
else
    winsock_dep = []
endif

fossil_media_lib = static_library('fossil-media',
    files('json.c', 'xml.c'),
    install: true,
    dependencies: [cc.find_library('m', required: false), winsock_dep],
    include_directories: dir)

fossil_media_dep = declare_dependency(
    link_with: [fossil_media_lib],
    include_directories: dir)
