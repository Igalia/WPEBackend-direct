#include "NativeSurface.h"

#include <glib.h>

#ifdef USE_X11
#include <X11/Xlib.h>

namespace
{
class NativeX11Surface final : public NativeSurface
{
  public:
    static std::unique_ptr<NativeSurface> createNativeX11Surface(EGLNativeDisplayType display, unsigned int width,
                                                                 unsigned int height) noexcept;
    ~NativeX11Surface();

  protected:
    void resizeUnderlyingWindow() noexcept override
    {
        XResizeWindow(static_cast<Display*>(m_display), reinterpret_cast<Window>(m_window), m_width, m_height);
    }

  private:
    NativeX11Surface(EGLNativeDisplayType display, unsigned int width, unsigned int height)
        : NativeSurface(display, width, height)
    {
    }
};

std::unique_ptr<NativeSurface> NativeX11Surface::createNativeX11Surface(EGLNativeDisplayType display,
                                                                        unsigned int width,
                                                                        unsigned int height) noexcept
{
    std::unique_ptr<NativeX11Surface> surface(new NativeX11Surface(display, width, height));

    int screen = DefaultScreen(display);
    XSetWindowAttributes attr = {};
    attr.background_pixel = BlackPixel(display, screen);
    Window wnd =
        XCreateWindow(static_cast<Display*>(display), RootWindow(display, screen), 0, 0, surface->m_width,
                      surface->m_height, 0, 0, InputOutput, DefaultVisual(display, screen), CWBackPixel, &attr);
    surface->m_window = reinterpret_cast<EGLNativeWindowType>(wnd);
    if (!surface->m_window)
    {
        g_critical("Cannot create X11 window for the native surface");
        return nullptr;
    }

    Atom deleteAtom = XInternAtom(static_cast<Display*>(display), "WM_DELETE_WINDOW", False);
    XSetWMProtocols(static_cast<Display*>(display), wnd, &deleteAtom, 1);

    XMapWindow(static_cast<Display*>(display), wnd);
    XFlush(static_cast<Display*>(display));

    g_message("Native X11 surface created (%dx%d)", surface->m_width, surface->m_height);
    return surface;
}

NativeX11Surface::~NativeX11Surface()
{
    if (m_display)
    {
        if (m_window)
        {
            XDestroyWindow(static_cast<Display*>(m_display), reinterpret_cast<Window>(m_window));
            m_window = 0;
        }

        XCloseDisplay(static_cast<Display*>(m_display));
        m_display = nullptr;
    }
}
} // namespace
#endif // USE_X11

#ifdef USE_WAYLAND
#include <cstring>
#include <wayland-egl.h>

namespace
{
class NativeWaylandSurface final : public NativeSurface
{
  public:
    static std::unique_ptr<NativeSurface> createNativeWaylandSurface(EGLNativeDisplayType display, unsigned int width,
                                                                     unsigned int height) noexcept;
    ~NativeWaylandSurface();

  protected:
    void resizeUnderlyingWindow() noexcept override
    {
        wl_egl_window_resize(reinterpret_cast<wl_egl_window*>(m_window), m_width, m_height, 0, 0);
    }

  private:
    NativeWaylandSurface(EGLNativeDisplayType display, unsigned int width, unsigned int height)
        : NativeSurface(display, width, height)
    {
    }

    wl_surface* m_wlSurface = nullptr;
};

std::unique_ptr<NativeSurface> NativeWaylandSurface::createNativeWaylandSurface(EGLNativeDisplayType display,
                                                                                unsigned int width,
                                                                                unsigned int height) noexcept
{
    std::unique_ptr<NativeWaylandSurface> surface(new NativeWaylandSurface(display, width, height));

    struct WaylandGlobal
    {
        wl_compositor* compositor;
        wl_shell* shell;

        ~WaylandGlobal()
        {
            if (compositor)
                wl_compositor_destroy(compositor);

            if (shell)
                wl_shell_destroy(shell);
        }
    } wlGlobal = {};

    wl_registry_listener wlRegistryListener = {
        .global =
            +[](void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t) {
                WaylandGlobal* global = static_cast<WaylandGlobal*>(data);
                if (std::strcmp(interface, wl_compositor_interface.name) == 0)
                    global->compositor =
                        static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
                else if (std::strcmp(interface, wl_shell_interface.name) == 0)
                    global->shell = static_cast<wl_shell*>(wl_registry_bind(registry, name, &wl_shell_interface, 1));
            },
        .global_remove = +[](void*, wl_registry*, uint32_t) {}};

    wl_registry* wlRegistry = wl_display_get_registry(static_cast<wl_display*>(display));
    wl_registry_add_listener(wlRegistry, &wlRegistryListener, &wlGlobal);
    wl_display_roundtrip(static_cast<wl_display*>(display));
    wl_registry_destroy(wlRegistry);

    if (!wlGlobal.compositor || !wlGlobal.shell)
    {
        g_critical("Cannot fetch Wayland compositor and/or shell");
        return nullptr;
    }

    surface->m_wlSurface = wl_compositor_create_surface(wlGlobal.compositor);
    if (!surface->m_wlSurface)
    {
        g_critical("Cannot create Wayland native surface");
        return nullptr;
    }

    surface->m_window = static_cast<EGLNativeWindowType>(
        wl_egl_window_create(surface->m_wlSurface, surface->m_width, surface->m_height));
    if (!surface->m_window)
    {
        g_critical("Cannot create Wayland EGL window for the native surface");
        return nullptr;
    }

    wl_shell_surface* wlShellSurface = wl_shell_get_shell_surface(wlGlobal.shell, surface->m_wlSurface);
    wl_shell_surface_set_toplevel(wlShellSurface);
    wl_shell_surface_set_title(wlShellSurface, "wpebackend-direct");

    wl_display_flush(static_cast<wl_display*>(display));
    g_message("Native Wayland surface created (%dx%d)", surface->m_width, surface->m_height);
    return surface;
}

NativeWaylandSurface::~NativeWaylandSurface()
{
    if (m_window)
    {
        wl_egl_window_destroy(reinterpret_cast<wl_egl_window*>(m_window));
        m_window = 0;
    }

    if (m_wlSurface)
    {
        wl_surface_destroy(m_wlSurface);
        m_wlSurface = nullptr;
    }

    if (m_display)
    {
        wl_display_disconnect(static_cast<wl_display*>(m_display));
        m_display = nullptr;
    }
}
} // namespace
#endif // USE_WAYLAND

NativeSurface::NativeSurface(EGLNativeDisplayType display, unsigned int width, unsigned int height) : m_display(display)
{
    if (width > m_width)
        m_width = width;

    if (height > m_height)
        m_height = height;
}

std::unique_ptr<NativeSurface> NativeSurface::createNativeSurface(EGLenum platform, EGLNativeDisplayType display,
                                                                  unsigned int width, unsigned int height) noexcept
{
#ifdef USE_WAYLAND
    if (platform == EGL_PLATFORM_WAYLAND_KHR)
        return NativeWaylandSurface::createNativeWaylandSurface(display, width, height);
#endif // USE_WAYLAND

#ifdef USE_X11
    if (platform == EGL_PLATFORM_X11_KHR)
        return NativeX11Surface::createNativeX11Surface(display, width, height);
#endif // USE_X11

    return nullptr;
}

void NativeSurface::resize(unsigned int width, unsigned int height) noexcept
{
    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    if ((width != m_width) || (height != m_height))
    {
        m_width = width;
        m_height = height;
        resizeUnderlyingWindow();
    }
}
