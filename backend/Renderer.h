#pragma once

#include <epoxy/egl.h>
#include <memory>

class Renderer final
{
  public:
    static std::unique_ptr<Renderer> createRenderer(EGLNativeDisplayType display, EGLNativeWindowType window) noexcept;
    ~Renderer();

    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    EGLDisplay getEGLDisplay() const noexcept
    {
        return m_display;
    }

    bool makeCurrent() const noexcept;
    bool swapBuffers() const noexcept;

  private:
    Renderer() = default;

    EGLDisplay m_display = EGL_NO_DISPLAY;
    EGLContext m_context = EGL_NO_CONTEXT;
    EGLSurface m_surface = EGL_NO_SURFACE;
};
