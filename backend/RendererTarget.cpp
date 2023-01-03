#include "RendererTarget.h"

#include <stdio.h>

wpe_renderer_backend_egl_target_interface* RendererTarget::getWPEInterface() noexcept
{
    static wpe_renderer_backend_egl_target_interface s_interface = {
        // void* create(wpe_renderer_backend_egl_target* target, int clientFd)
        +[](wpe_renderer_backend_egl_target* target, int clientFd) -> void* {
            return new RendererTarget(target, clientFd);
        },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<RendererTarget*>(data); },
        // void initialize(void* targetData, void* backendData, uint32_t width, uint32_t height)
        +[](void* targetData, void* backendData, uint32_t width, uint32_t height) {
            static_cast<RendererTarget*>(targetData)->init(static_cast<RendererBackend*>(backendData), width, height);
        },
        // EGLNativeWindowType get_native_window(void* data)
        +[](void* data) -> EGLNativeWindowType { return static_cast<RendererTarget*>(data)->getNativeWindow(); },
        // void resize(void* data, uint32_t width, uint32_t height)
        +[](void* data, uint32_t width, uint32_t height) { static_cast<RendererTarget*>(data)->resize(width, height); },
        // void frame_will_render(void* data)
        +[](void* data) { static_cast<RendererTarget*>(data)->frameWillRender(); },
        // void frame_rendered(void* data)
        +[](void* data) { static_cast<RendererTarget*>(data)->frameRendered(); },
        // void deinitialize(void* data)
        +[](void* data) { static_cast<RendererTarget*>(data)->shut(); }, nullptr, nullptr, nullptr};

    return &s_interface;
}

void RendererTarget::init(RendererBackend* backend, uint32_t width, uint32_t height) noexcept
{
    if (m_backend || m_nativeSurface)
    {
        printf("RendererTarget is already initialized\n");
        return;
    }

    m_backend = backend;
    m_nativeSurface =
        NativeSurface::createNativeSurface(m_backend->getPlatform(), m_backend->getDisplay(), width, height);
    if (!m_nativeSurface)
    {
        printf("Cannot create RendererTarget native surface\n");
        shut();
        return;
    }

    m_eglDisplay = eglGetDisplay(m_backend->getDisplay());
    if (!m_eglDisplay)
    {
        printf("Cannot connect to any EGL display\n");
        shut();
        return;
    }

    EGLint major = 0;
    EGLint minor = 0;
    if (!eglInitialize(m_eglDisplay, &major, &minor))
    {
        printf("Cannot initialize EGL display\n");
        shut();
        return;
    }
    printf("EGL %d.%d initialized\n", major, minor);

    if (!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Cannot bind EGL ES API\n");
        shut();
        return;
    }

    EGLint configAttribs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE};

    EGLConfig configs[1] = {0};
    EGLint numConfigs = 0;
    if (!eglChooseConfig(m_eglDisplay, configAttribs, configs, 1, &numConfigs) || (numConfigs < 1))
    {
        printf("Cannot find any EGL ES compatible configuration\n");
        shut();
        return;
    }

    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    m_eglContext = eglCreateContext(m_eglDisplay, configs[0], EGL_NO_CONTEXT, contextAttribs);
    if (!m_eglContext)
    {
        printf("Cannot create EGL ES context\n");
        shut();
        return;
    }

    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, configs[0], m_nativeSurface->getWindow(), nullptr);
    if (!m_eglSurface)
    {
        printf("Cannot create EGL ES surface\n");
        shut();
        return;
    }

    printf("RendererTarget initialized\n");
}

void RendererTarget::shut() noexcept
{
    if (m_eglDisplay)
    {
        eglMakeCurrent(m_eglDisplay, nullptr, nullptr, nullptr);

        if (m_eglSurface)
        {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
            m_eglSurface = EGL_NO_SURFACE;
        }

        if (m_eglContext)
        {
            eglDestroyContext(m_eglDisplay, m_eglContext);
            m_eglContext = EGL_NO_CONTEXT;
        }

        eglTerminate(m_eglDisplay);
        m_eglDisplay = EGL_NO_DISPLAY;
    }

    m_backend = nullptr;
    m_nativeSurface = nullptr;
}

void RendererTarget::frameWillRender() const noexcept
{
    if (m_eglDisplay)
        eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
}

void RendererTarget::frameRendered() const noexcept
{
    if (m_eglDisplay)
    {
        // TODO
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
    }
}

void RendererTarget::handleMessage(const ipc::Message& /*message*/) noexcept
{
    // Renderer messages received on WPEWebProcess side from the client process side
}
