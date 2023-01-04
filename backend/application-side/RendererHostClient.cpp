#include "RendererHost.h"

void RendererHostClient::handleMessage(IPC::Channel& /*channel*/, const IPC::Message& /*message*/) noexcept
{
    // Messages received on application process side from RendererBackendEGL on WPEWebProcess side
}

void RendererHostClient::handlePeerClosed(IPC::Channel& /*channel*/) noexcept
{
    if (m_host)
        m_host->removeClient(this);
}
