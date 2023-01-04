#pragma once

#include "../common/ipc.h"

#include <wpe/wpe.h>

class ViewBackend final : private IPC::MessageHandler
{
  public:
    static wpe_view_backend_interface* getWPEInterface() noexcept;

    ~ViewBackend() = default;

    ViewBackend(ViewBackend&&) = delete;
    ViewBackend& operator=(ViewBackend&&) = delete;
    ViewBackend(const ViewBackend&) = delete;
    ViewBackend& operator=(const ViewBackend&) = delete;

    void initialize();

  private:
    wpe_view_backend* m_wpeViewBackend = nullptr;
    IPC::Channel m_ipcChannel;

    ViewBackend(wpe_view_backend* wpeViewBackend) noexcept : m_wpeViewBackend(wpeViewBackend), m_ipcChannel(*this)
    {
    }

    void handleMessage(IPC::Channel& channel, const IPC::Message& message) noexcept override;
};
