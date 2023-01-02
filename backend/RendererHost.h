#pragma once

#include "ipc.h"

#include <wpe/wpe.h>

class RendererHost final : private ipc::MessageHandler
{
  public:
    static RendererHost& getSingleton() noexcept;
    static wpe_renderer_host_interface* getWPEInterface() noexcept;

    ~RendererHost() = default;

    RendererHost(RendererHost&&) = delete;
    RendererHost& operator=(RendererHost&&) = delete;
    RendererHost(const RendererHost&) = delete;
    RendererHost& operator=(const RendererHost&) = delete;

  private:
    ipc::Server m_ipcServer;
    RendererHost() noexcept : m_ipcServer(*this)
    {
    }

    void handleMessage(const ipc::Message& message) noexcept override;
};
