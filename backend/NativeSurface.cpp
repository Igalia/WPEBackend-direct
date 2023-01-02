#include "NativeSurface.h"

#include <stdio.h>

#ifdef USE_X11
#include <X11/Xlib.h>

namespace
{
class NativeX11Surface final : public NativeSurface
{
  public:
    static std::unique_ptr<NativeSurface> createNativeX11Surface(unsigned int width, unsigned int height) noexcept;
    ~NativeX11Surface();

    EGLenum getPlatform() const noexcept override
    {
        return EGL_PLATFORM_X11_KHR;
    }

  protected:
    void resizeUnderlyingWindow() noexcept override
    {
        XResizeWindow(static_cast<Display*>(m_display), reinterpret_cast<Window>(m_window), m_width, m_height);
    }

  private:
    NativeX11Surface(unsigned int width, unsigned int height) : NativeSurface(width, height)
    {
    }
};

std::unique_ptr<NativeSurface> NativeX11Surface::createNativeX11Surface(unsigned int width,
                                                                        unsigned int height) noexcept
{
    Display* display = XOpenDisplay(nullptr);
    if (!display)
    {
        printf("Cannot connect to X11 display\n");
        return nullptr;
    }

    std::unique_ptr<NativeX11Surface> surface(new NativeX11Surface(width, height));
    surface->m_display = display;

    int screen = DefaultScreen(display);
    XSetWindowAttributes attr = {};
    attr.background_pixel = BlackPixel(display, screen);
    Window wnd = XCreateWindow(display, RootWindow(display, screen), 0, 0, surface->m_width, surface->m_height, 0, 0,
                               InputOutput, DefaultVisual(display, screen), CWBackPixel, &attr);
    surface->m_window = reinterpret_cast<EGLNativeWindowType>(wnd);
    if (!surface->m_window)
    {
        printf("Cannot create X11 window for the native surface\n");
        return nullptr;
    }

    Atom deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, wnd, &deleteAtom, 1);

    XMapWindow(display, wnd);
    XFlush(display);

    printf("Native X11 surface created (%dx%d)\n", surface->m_width, surface->m_height);
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
    static std::unique_ptr<NativeSurface> createNativeWaylandSurface(unsigned int width, unsigned int height) noexcept;
    ~NativeWaylandSurface();

    EGLenum getPlatform() const noexcept override
    {
        return EGL_PLATFORM_WAYLAND_KHR;
    }

  protected:
    void resizeUnderlyingWindow() noexcept override
    {
        wl_egl_window_resize(reinterpret_cast<wl_egl_window*>(m_window), m_width, m_height, 0, 0);
    }

  private:
    NativeWaylandSurface(unsigned int width, unsigned int height) : NativeSurface(width, height)
    {
    }

    wl_surface* m_wlSurface = nullptr;
};

std::unique_ptr<NativeSurface> NativeWaylandSurface::createNativeWaylandSurface(unsigned int width,
                                                                                unsigned int height) noexcept
{
    wl_display* display = wl_display_connect(nullptr);
    if (!display)
    {
        printf("Cannot connect to Wayland display\n");
        return nullptr;
    }

    std::unique_ptr<NativeWaylandSurface> surface(new NativeWaylandSurface(width, height));
    surface->m_display = display;

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

    wl_registry* wlRegistry = wl_display_get_registry(display);
    wl_registry_add_listener(wlRegistry, &wlRegistryListener, &wlGlobal);
    wl_display_roundtrip(display);
    wl_registry_destroy(wlRegistry);

    if (!wlGlobal.compositor || !wlGlobal.shell)
    {
        printf("Cannot fetch Wayland compositor and/or shell\n");
        return nullptr;
    }

    surface->m_wlSurface = wl_compositor_create_surface(wlGlobal.compositor);
    if (!surface->m_wlSurface)
    {
        printf("Cannot create Wayland native surface\n");
        return nullptr;
    }

    surface->m_window = static_cast<EGLNativeWindowType>(
        wl_egl_window_create(surface->m_wlSurface, surface->m_width, surface->m_height));
    if (!surface->m_window)
    {
        printf("Cannot create Wayland EGL window for the native surface\n");
        return nullptr;
    }

    wl_shell_surface* wlShellSurface = wl_shell_get_shell_surface(wlGlobal.shell, surface->m_wlSurface);
    wl_shell_surface_set_toplevel(wlShellSurface);
    wl_shell_surface_set_title(wlShellSurface, "wpebackend-direct");

    wl_display_flush(display);
    printf("Native Wayland surface created (%dx%d)\n", surface->m_width, surface->m_height);
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

NativeSurface::NativeSurface(unsigned int width, unsigned int height)
{
    if (width > m_width)
        m_width = width;

    if (height > m_height)
        m_height = height;
}

std::unique_ptr<NativeSurface> NativeSurface::createNativeSurface(unsigned int width, unsigned int height) noexcept
{
    std::unique_ptr<NativeSurface> surface;

#ifdef USE_WAYLAND
    surface = NativeWaylandSurface::createNativeWaylandSurface(width, height);
    if (surface)
        return surface;
#endif // USE_WAYLAND

#ifdef USE_X11
    surface = NativeX11Surface::createNativeX11Surface(width, height);
    if (surface)
        return surface;
#endif // USE_X11

    return surface;
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
