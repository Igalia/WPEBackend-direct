#include "RendererBackend.h"

wpe_renderer_backend_egl_interface* RendererBackend::getWPEInterface() noexcept
{
    static wpe_renderer_backend_egl_interface s_interface = {
        // void* create(int clientFd)
        +[](int clientFd) -> void* { return new RendererBackend(clientFd); },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<RendererBackend*>(data); },
        // EGLNativeDisplayType get_native_display(void* data)
        +[](void*) -> EGLNativeDisplayType { return EGL_DEFAULT_DISPLAY; },
        // uint32_t get_platform(void* data)
        +[](void*) -> uint32_t { return EGL_PLATFORM_SURFACELESS_MESA; }, nullptr, nullptr, nullptr};

    return &s_interface;
}

void RendererBackend::handleMessage(const ipc::Message& /*message*/) noexcept
{
    // Renderer messages received on WPEWebProcess side from the client process side
}
