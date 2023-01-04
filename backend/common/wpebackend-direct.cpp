#include "../wpebackend-direct.h"

#include "../application-side/RendererHost.h"
#include "../application-side/ViewBackend.h"
#include "../wpewebprocess-side/RendererBackendEGL.h"
#include "../wpewebprocess-side/RendererBackendEGLTarget.h"

#include <cstring>

/**
 * Components architecture per process:
 *
 *                                             ___________________________
 *  ____________________________              | Application Process       |
 * | WPEWebProcess              |             |                           |
 * |                            |     IPC     | RendererHost              |
 * | RendererBackendEGL       <-------------------> |- RendererHostClient |
 * |                            |    /------------> |- RendererHostClient |
 * |                            |    |        |                           |
 * |                            |    |        |                           |
 * | RendererBackendEGLTarget <------|----------> ViewBackend             |
 * | RendererBackendEGLTarget <------|----------> ViewBackend             |
 * | RendererBackendEGLTarget <------|----------> ViewBackend             |
 * |____________________________|    |        |                           |
 *                                   |    /-----> ViewBackend             |
 *  ____________________________     |    | /---> ViewBackend             |
 * | WPEWebProcess              |    |    | | |___________________________|
 * |                            |    |    | |
 * | RendererBackendEGL       <------/    | |
 * |                            |         | |
 * | RendererBackendEGLTarget <-----------/ |
 * | RendererBackendEGLTarget <-------------/
 * |____________________________|
 *
 */

extern "C"
{
    __attribute__((visibility("default"))) struct wpe_loader_interface _wpe_loader_interface = {
        +[](const char* name) -> void* {
            if (std::strcmp(name, "_wpe_renderer_host_interface") == 0)
                return RendererHost::getWPEInterface();

            if (std::strcmp(name, "_wpe_renderer_backend_egl_interface") == 0)
                return RendererBackendEGL::getWPEInterface();

            if (std::strcmp(name, "_wpe_renderer_backend_egl_target_interface") == 0)
                return RendererBackendEGLTarget::getWPEInterface();

            if (std::strcmp(name, "_wpe_renderer_backend_egl_offscreen_target_interface") == 0)
            {
                static wpe_renderer_backend_egl_offscreen_target_interface s_interface = {
                    +[]() -> void* { return nullptr; },
                    +[](void*) {},
                    +[](void*, void*) {},
                    +[](void*) -> EGLNativeWindowType { return nullptr; },
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr};
                return &s_interface;
            }

            return nullptr;
        },
        nullptr, nullptr, nullptr, nullptr};
}

__attribute__((visibility("default"))) wpe_view_backend* wpe_direct_view_backend_create()
{
    return wpe_view_backend_create_with_backend_interface(ViewBackend::getWPEInterface(), nullptr);
}
