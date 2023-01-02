#include "Backend.h"
#include "RendererHost.h"

#include <cstring>
#include <wpe/wpe-egl.h>

namespace
{
auto s_backend = Backend::createBackend(960, 540);

wpe_renderer_backend_egl_interface renderer_backend_egl_interface = {
    +[](int) -> void* { return s_backend.get(); },
    +[](void*) {},
    +[](void* backend) -> EGLNativeDisplayType { return static_cast<Backend*>(backend)->getDisplay(); },
    +[](void* backend) -> uint32_t { return static_cast<Backend*>(backend)->getPlatform(); },
    nullptr,
    nullptr,
    nullptr};

wpe_renderer_backend_egl_target_interface renderer_backend_egl_target_interface = {
    +[](wpe_renderer_backend_egl_target*, int) -> void* { return s_backend.get(); },
    +[](void*) {},
    +[](void*, void*, uint32_t, uint32_t) {},
    +[](void* backend) -> EGLNativeWindowType { return static_cast<Backend*>(backend)->getWindow(); },
    +[](void*, uint32_t, uint32_t) {},
    +[](void* backend) { static_cast<Backend*>(backend)->frameWillRender(); },
    +[](void* backend) { static_cast<Backend*>(backend)->frameRendered(); },
    +[](void*) {},
    nullptr,
    nullptr,
    nullptr};

wpe_renderer_backend_egl_offscreen_target_interface renderer_backend_egl_offscreen_target_interface = {
    +[]() -> void* { return nullptr; },
    +[](void*) {},
    +[](void*, void*) {},
    +[](void*) -> EGLNativeWindowType { return nullptr; },
    nullptr,
    nullptr,
    nullptr,
    nullptr};
} // namespace

extern "C"
{
    __attribute__((visibility("default"))) struct wpe_loader_interface _wpe_loader_interface = {
        +[](const char* name) -> void* {
            if (std::strcmp(name, "_wpe_renderer_host_interface") == 0)
                return RendererHost::getWPEInterface();

            if (std::strcmp(name, "_wpe_renderer_backend_egl_interface") == 0)
                return &renderer_backend_egl_interface;

            if (std::strcmp(name, "_wpe_renderer_backend_egl_target_interface") == 0)
                return &renderer_backend_egl_target_interface;

            if (std::strcmp(name, "_wpe_renderer_backend_egl_offscreen_target_interface") == 0)
                return &renderer_backend_egl_offscreen_target_interface;

            return nullptr;
        },
        nullptr, nullptr, nullptr, nullptr};
}
