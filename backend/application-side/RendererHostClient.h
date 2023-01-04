#pragma once

#include "../common/ipc.h"

class RendererHost;

class RendererHostClient final : private IPC::MessageHandler
{
  public:
    int detachIPCChannelPeerFd() noexcept
    {
        return m_ipcChannel.detachPeerFd();
    }

  private:
    friend class RendererHost;

    RendererHost* m_host = nullptr;
    IPC::Channel m_ipcChannel;

    RendererHostClient(RendererHost* host) noexcept : m_host(host), m_ipcChannel(*this)
    {
    }

    void handleMessage(IPC::Channel& channel, const IPC::Message& message) noexcept override;
    void handlePeerClosed(IPC::Channel& channel) noexcept override;
};
