#include "RendererBackend.h"

#include <stdio.h>

#ifdef USE_WAYLAND
#include <wayland-client.h>
#endif // USE_WAYLAND

#ifdef USE_X11
#include <X11/Xlib.h>
#endif // USE_X11

wpe_renderer_backend_egl_interface* RendererBackend::getWPEInterface() noexcept
{
    static wpe_renderer_backend_egl_interface s_interface = {
        // void* create(int clientFd)
        +[](int clientFd) -> void* { return new RendererBackend(clientFd); },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<RendererBackend*>(data); },
        // EGLNativeDisplayType get_native_display(void* data)
        +[](void* data) -> EGLNativeDisplayType { return static_cast<RendererBackend*>(data)->getDisplay(); },
        // uint32_t get_platform(void* data)
        +[](void* data) -> uint32_t { return static_cast<RendererBackend*>(data)->getPlatform(); }, nullptr, nullptr,
        nullptr};

    return &s_interface;
}

RendererBackend::RendererBackend(int clientFd) noexcept : m_ipcClient(*this, clientFd)
{
#ifdef USE_WAYLAND
    wl_display* waylandDisplay = wl_display_connect(nullptr);
    if (waylandDisplay)
    {
        m_display = waylandDisplay;
        m_platform = EGL_PLATFORM_WAYLAND_KHR;
        printf("Using Wayland platform\n");
        return;
    }
#endif // USE_WAYLAND

#ifdef USE_X11
    Display* x11Display = XOpenDisplay(nullptr);
    if (x11Display)
    {
        m_display = x11Display;
        m_platform = EGL_PLATFORM_X11_KHR;
        printf("Using X11 platform\n");
    }
#endif // USE_X11
}

RendererBackend::~RendererBackend()
{
    if (m_display)
    {
#ifdef USE_WAYLAND
        if (m_platform == EGL_PLATFORM_WAYLAND_KHR)
            wl_display_disconnect(static_cast<wl_display*>(m_display));
#endif // USE_WAYLAND

#ifdef USE_X11
        if (m_platform == EGL_PLATFORM_X11_KHR)
            XCloseDisplay(static_cast<Display*>(m_display));
#endif // USE_X11
    }

    m_display = EGL_DEFAULT_DISPLAY;
    m_platform = EGL_PLATFORM_SURFACELESS_MESA;
}

void RendererBackend::handleMessage(const ipc::Message& /*message*/) noexcept
{
    // Renderer messages received on WPEWebProcess side from the client process side
}
