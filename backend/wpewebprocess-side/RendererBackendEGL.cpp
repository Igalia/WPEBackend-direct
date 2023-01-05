#include "RendererBackendEGL.h"

#ifdef USE_WAYLAND
#include <wayland-client.h>
#endif // USE_WAYLAND

#ifdef USE_X11
#include <X11/Xlib.h>
#endif // USE_X11

wpe_renderer_backend_egl_interface* RendererBackendEGL::getWPEInterface() noexcept
{
    static wpe_renderer_backend_egl_interface s_interface = {
        // void* create(int peerFd)
        +[](int peerFd) -> void* { return new RendererBackendEGL(peerFd); },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<RendererBackendEGL*>(data); },
        // EGLNativeDisplayType get_native_display(void* data)
        +[](void* data) -> EGLNativeDisplayType { return static_cast<RendererBackendEGL*>(data)->getDisplay(); },
        // uint32_t get_platform(void* data)
        +[](void* data) -> uint32_t { return static_cast<RendererBackendEGL*>(data)->getPlatform(); }, nullptr, nullptr,
        nullptr};

    return &s_interface;
}

RendererBackendEGL::RendererBackendEGL(int rendererHostClientFd) noexcept : m_ipcChannel(*this, rendererHostClientFd)
{
#ifdef USE_WAYLAND
    wl_display* waylandDisplay = wl_display_connect(nullptr);
    if (waylandDisplay)
    {
        m_display = waylandDisplay;
        m_platform = EGL_PLATFORM_WAYLAND_KHR;
        g_message("Using Wayland platform");
        return;
    }
#endif // USE_WAYLAND

#ifdef USE_X11
    Display* x11Display = XOpenDisplay(nullptr);
    if (x11Display)
    {
        m_display = x11Display;
        m_platform = EGL_PLATFORM_X11_KHR;
        g_message("Using X11 platform");
    }
#endif // USE_X11
}

RendererBackendEGL::~RendererBackendEGL()
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

    m_display = EGL_NO_DISPLAY;
    m_platform = 0;
}

void RendererBackendEGL::handleMessage(IPC::Channel& /*channel*/, const IPC::Message& /*message*/) noexcept
{
    // Messages received on WPEWebProcess side from RendererHostClient on application process side
}
