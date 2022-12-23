build_args = ['-DEGL_NO_PLATFORM_SPECIFIC_TYPES']

wpe_dep = dependency('wpe-1.0', version: '>= 1.12', required: true)
epoxy_dep = dependency('epoxy', version: '>= 1.5', required: true)
build_deps = [wpe_dep, epoxy_dep]

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

build_src = ['Backend.cpp', 'NativeSurface.cpp', 'Renderer.cpp', 'wpebackend-direct.cpp']

shared_library('wpebackend-direct', build_src,
               dependencies: build_deps,
               cpp_args: build_args)
