#pragma once

#include "NativeSurface.h"
#include "RendererBackend.h"

class RendererTarget final : private ipc::MessageHandler
{
  public:
    static wpe_renderer_backend_egl_target_interface* getWPEInterface() noexcept;

    ~RendererTarget()
    {
        shut();
    }

    RendererTarget(RendererTarget&&) = delete;
    RendererTarget& operator=(RendererTarget&&) = delete;
    RendererTarget(const RendererTarget&) = delete;
    RendererTarget& operator=(const RendererTarget&) = delete;

    EGLNativeWindowType getNativeWindow() const noexcept
    {
        return m_nativeSurface ? m_nativeSurface->getWindow() : nullptr;
    }

    void init(RendererBackend* backend, uint32_t width, uint32_t height) noexcept;
    void shut() noexcept;

    void resize(uint32_t width, uint32_t height) noexcept
    {
        if (m_nativeSurface)
            m_nativeSurface->resize(width, height);
    }

    void frameWillRender() const noexcept;
    void frameRendered() const noexcept;

  private:
    wpe_renderer_backend_egl_target* m_target = nullptr;
    ipc::Client m_ipcClient;

    RendererTarget(wpe_renderer_backend_egl_target* target, int clientFd) noexcept
        : m_target(target), m_ipcClient(*this, clientFd)
    {
    }

    void handleMessage(const ipc::Message& message) noexcept override;

    RendererBackend* m_backend = nullptr;
    std::unique_ptr<NativeSurface> m_nativeSurface;

    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLContext m_eglContext = EGL_NO_CONTEXT;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;
};
