#pragma once

#include "NativeSurface.h"
#include "RendererBackendEGL.h"

class RendererBackendEGLTarget final : private IPC::MessageHandler
{
  public:
    static wpe_renderer_backend_egl_target_interface* getWPEInterface() noexcept;

    ~RendererBackendEGLTarget()
    {
        shut();
    }

    RendererBackendEGLTarget(RendererBackendEGLTarget&&) = delete;
    RendererBackendEGLTarget& operator=(RendererBackendEGLTarget&&) = delete;
    RendererBackendEGLTarget(const RendererBackendEGLTarget&) = delete;
    RendererBackendEGLTarget& operator=(const RendererBackendEGLTarget&) = delete;

    EGLNativeWindowType getNativeWindow() const noexcept
    {
        return m_nativeSurface ? m_nativeSurface->getWindow() : nullptr;
    }

    void init(RendererBackendEGL* backend, uint32_t width, uint32_t height) noexcept;
    void shut() noexcept;

    void resize(uint32_t width, uint32_t height) noexcept
    {
        if (m_nativeSurface)
            m_nativeSurface->resize(width, height);
    }

    void frameWillRender() const noexcept;
    void frameRendered() const noexcept;

  private:
    wpe_renderer_backend_egl_target* m_wpeTarget = nullptr;
    IPC::Channel m_ipcChannel;

    RendererBackendEGLTarget(wpe_renderer_backend_egl_target* wpeTarget, int viewBackendFd) noexcept
        : m_wpeTarget(wpeTarget), m_ipcChannel(*this, viewBackendFd)
    {
    }

    void handleMessage(IPC::Channel& channel, const IPC::Message& message) noexcept override;

    RendererBackendEGL* m_backend = nullptr;
    std::unique_ptr<NativeSurface> m_nativeSurface;
};
