#pragma once

#include "../common/ipc.h"

#include <epoxy/egl.h>
#include <wpe/wpe-egl.h>

class RendererBackendEGL final : private IPC::MessageHandler
{
  public:
    static wpe_renderer_backend_egl_interface* getWPEInterface() noexcept;

    ~RendererBackendEGL();

    RendererBackendEGL(RendererBackendEGL&&) = delete;
    RendererBackendEGL& operator=(RendererBackendEGL&&) = delete;
    RendererBackendEGL(const RendererBackendEGL&) = delete;
    RendererBackendEGL& operator=(const RendererBackendEGL&) = delete;

    EGLNativeDisplayType getDisplay() const noexcept
    {
        return m_display;
    }

    EGLenum getPlatform() const noexcept
    {
        return m_platform;
    }

  private:
    RendererBackendEGL(int rendererHostClientFd) noexcept;
    void handleMessage(IPC::Channel& channel, const IPC::Message& message) noexcept override;

    IPC::Channel m_ipcChannel;
    EGLNativeDisplayType m_display = EGL_NO_DISPLAY;
    EGLenum m_platform = 0;
};
