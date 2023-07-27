# WPEBackend Direct

[![License](https://img.shields.io/badge/License-BSD_2--Clause-orange.svg)](./LICENSE)

This code provides a very simple rendering backend for
[WPEWebkit](https://wpewebkit.org/) on Linux.

The unique purpose of this backend is to render the output of the WPEWebkit
compositor into an X11 or Wayland surface using EGL and directly from the
WPEWebProcess process. There is no inter-processes exchange of any buffer to
the user application.

This backend can help debugging the WPEWebkit rendering system by offering a
direct view of the compositor output.

## Build dependencies

To build the backend you will need the development dependencies for
libWPEWebKit-1.0 (minimum version 2.38) and libwpe-1.0 (minimum version 1.14).

You can install those dependencies from your OS packaging system or you can
build them yourself (preferred way as it would include specific patches).

### On Ubuntu Jammy

If you are using Ubuntu Jammy (22.04 LTS), you can call directly the
*scripts/webkit* script to build locally all dependencies and open a shell with
all needed environment variables set to proceed with the backend build.

```shell
cd scripts
./webkit
```

### On other Linux distributions

If you are using another Linux distribution, you can use the provided
*script/Dockerfile* to build a container with all the dependencies inside.

To build the container call:

```shell
cd scripts
docker build --force-rm --no-cache --tag webkit-dev .
```

And to launch the build environment (with an X11 server on the host):

```shell
docker run --rm -it -e DISPLAY=$DISPLAY -v ~/.Xauthority:/root/.Xauthority -v /tmp/.X11-unix:/tmp/.X11-unix -v .:/root/code webkit-dev
```

You will need to call `xhost +` on the host to authorize the docker container
to access the host X11 server.

If running from inside a docker container, and depending on your graphic card,
the hardware acceleration may be difficult to configure. In this case, a local
build of the dependencies is preferable. The *scripts/webkit* script should
work on all Linux distributions except for the `install_build_deps` function
which is specific to Ubuntu Jammy but this function may be easily ported to
different distributions.

## Build the backend and the sample application

Once inside the build environment (initialized with the *scripts/webkit*
script if you've built the dependencies yourself), call:

```shell
meson setup build
ninja -C build
```

It will build the backend library into *build/backend* and the sample
application into *build/webview*. Before launching the `webview` application
you must ensure that `LD_LIBRARY_PATH` environment variable points to the
*build/backend* directory.

## Use the direct backend to help debugging WPEWebkit rendering system

You need to have built the WPEWebkit dependencies yourself. Just define the
environment variable `WEBKIT2_PAUSE_WEB_PROCESS_ON_LAUNCH=1` before launching
the `webview` sample application and then connect your debugger to the
*WPEWebProcess*.

If you have built *libWPEWebKit-1.0.so* in debug mode, you may need to replace
*scripts/webkit-2.38/dist/lib/libWPEWebKit-1.0.so.3.18.9* by
*libWPEWebKit-1.0.so.debug* as the *scripts/webkit* script strips the library
debug symbols by default to prevent huge loading times when not needing to
directly debug the WPEWebkit library.
