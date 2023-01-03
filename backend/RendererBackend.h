#pragma once

#include "ipc.h"

#include <epoxy/egl.h>
#include <wpe/wpe-egl.h>

class RendererBackend final : private ipc::MessageHandler
{
  public:
    static wpe_renderer_backend_egl_interface* getWPEInterface() noexcept;

    ~RendererBackend();

    RendererBackend(RendererBackend&&) = delete;
    RendererBackend& operator=(RendererBackend&&) = delete;
    RendererBackend(const RendererBackend&) = delete;
    RendererBackend& operator=(const RendererBackend&) = delete;

    EGLNativeDisplayType getDisplay() const noexcept
    {
        return m_display;
    }

    EGLenum getPlatform() const noexcept
    {
        return m_platform;
    }

  private:
    RendererBackend(int clientFd) noexcept;
    void handleMessage(const ipc::Message& message) noexcept override;

    ipc::Client m_ipcClient;
    EGLNativeDisplayType m_display = EGL_DEFAULT_DISPLAY;
    EGLenum m_platform = EGL_PLATFORM_SURFACELESS_MESA;
};
