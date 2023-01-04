build_args = ['-DEGL_NO_PLATFORM_SPECIFIC_TYPES']

wpe_dep = dependency('wpe-1.0', version: '>= 1.12', required: true)
epoxy_dep = dependency('epoxy', version: '>= 1.5', required: true)
gio_dep = dependency('gio-2.0', version: '>=2.6', required: true)
build_deps = [wpe_dep, epoxy_dep, gio_dep]

x11_dep = dependency('x11', required: false)
if x11_dep.found()
    build_args += '-DUSE_X11'
    build_deps += x11_dep
endif

wlegl_dep = dependency('wayland-egl', required: false)
if wlegl_dep.found()
    build_args += '-DUSE_WAYLAND'
    build_deps += wlegl_dep
elif not x11_dep.found()
    error('You need at least one Window library: X11 or Wayland')
endif

build_src = [
    'application-side/RendererHost.cpp',
    'application-side/RendererHostClient.cpp',
    'application-side/ViewBackend.cpp',
    'common/ipc.cpp',
    'common/wpebackend-direct.cpp',
    'wpewebprocess-side/NativeSurface.cpp',
    'wpewebprocess-side/RendererBackendEGL.cpp',
    'wpewebprocess-side/RendererBackendEGLTarget.cpp']

wpebackend_lib = shared_library('wpebackend-direct', build_src,
                                dependencies: build_deps,
                                cpp_args: build_args)

wpebackend_dep = declare_dependency(compile_args: build_args,
                                    dependencies: build_deps,
                                    include_directories: '.',
                                    link_with: wpebackend_lib)
