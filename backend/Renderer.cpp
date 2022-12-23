#include "Renderer.h"

#include <stdio.h>

std::unique_ptr<Renderer> Renderer::createRenderer(EGLNativeDisplayType display, EGLNativeWindowType window) noexcept
{
    std::unique_ptr<Renderer> rdr(new Renderer());

    rdr->m_display = eglGetDisplay(display);
    if (!rdr->m_display)
    {
        printf("Cannot connect to any EGL display\n");
        return nullptr;
    }

    EGLint major = 0;
    EGLint minor = 0;
    if (!eglInitialize(rdr->m_display, &major, &minor))
    {
        printf("Cannot initialize EGL display\n");
        return nullptr;
    }
    printf("EGL %d.%d initialized\n", major, minor);

    if (!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Cannot bind EGL ES API\n");
        return nullptr;
    }

    EGLint configAttribs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE};

    EGLConfig configs[1] = {0};
    EGLint numConfigs = 0;
    if (!eglChooseConfig(rdr->m_display, configAttribs, configs, 1, &numConfigs) || (numConfigs < 1))
    {
        printf("Cannot find any EGL ES compatible configuration\n");
        return nullptr;
    }

    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    rdr->m_context = eglCreateContext(rdr->m_display, configs[0], EGL_NO_CONTEXT, contextAttribs);
    if (!rdr->m_context)
    {
        printf("Cannot create EGL ES context\n");
        return nullptr;
    }

    rdr->m_surface = eglCreateWindowSurface(rdr->m_display, configs[0], window, nullptr);
    if (!rdr->m_surface)
    {
        printf("Cannot create EGL ES surface\n");
        return nullptr;
    }

    printf("Renderer initialized\n");
    return rdr;
}

Renderer::~Renderer()
{
    if (m_display)
    {
        eglMakeCurrent(m_display, nullptr, nullptr, nullptr);

        if (m_surface)
        {
            eglDestroySurface(m_display, m_surface);
            m_surface = EGL_NO_SURFACE;
        }

        if (m_context)
        {
            eglDestroyContext(m_display, m_context);
            m_context = EGL_NO_CONTEXT;
        }

        eglTerminate(m_display);
        m_display = EGL_NO_DISPLAY;
    }
}

bool Renderer::makeCurrent() const noexcept
{
    return eglMakeCurrent(m_display, m_surface, m_surface, m_context);
}

bool Renderer::swapBuffers() const noexcept
{
    return eglSwapBuffers(m_display, m_surface);
}
