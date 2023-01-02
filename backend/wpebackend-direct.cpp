#include "RendererBackend.h"
#include "RendererHost.h"
#include "RendererTarget.h"

#include <cstring>

extern "C"
{
    __attribute__((visibility("default"))) struct wpe_loader_interface _wpe_loader_interface = {
        +[](const char* name) -> void* {
            if (std::strcmp(name, "_wpe_renderer_host_interface") == 0)
                return RendererHost::getWPEInterface();

            if (std::strcmp(name, "_wpe_renderer_backend_egl_interface") == 0)
                return RendererBackend::getWPEInterface();

            if (std::strcmp(name, "_wpe_renderer_backend_egl_target_interface") == 0)
                return RendererTarget::getWPEInterface();

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
