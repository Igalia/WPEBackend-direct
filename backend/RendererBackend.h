#pragma once

#include "ipc.h"

#include <epoxy/egl.h>
#include <wpe/wpe-egl.h>

class RendererBackend final : private ipc::MessageHandler
{
  public:
    static wpe_renderer_backend_egl_interface* getWPEInterface() noexcept;

    ~RendererBackend() = default;

    RendererBackend(RendererBackend&&) = delete;
    RendererBackend& operator=(RendererBackend&&) = delete;
    RendererBackend(const RendererBackend&) = delete;
    RendererBackend& operator=(const RendererBackend&) = delete;

  private:
    ipc::Client m_ipcClient;
    RendererBackend(int clientFd) noexcept : m_ipcClient(*this, clientFd)
    {
    }

    void handleMessage(const ipc::Message& message) noexcept override;
};
