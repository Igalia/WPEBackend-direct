#include "RendererHost.h"

wpe_renderer_host_interface* RendererHost::getWPEInterface() noexcept
{
    static wpe_renderer_host_interface s_interface = {
        // void* create()
        +[]() -> void* { return new RendererHost(); },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<RendererHost*>(data); },
        // int create_client(void* data) => called each time a new WPEWebProcess needs to be launched, it fetches the
        //                                  IPC client file descriptor used later from the WPEWebProcess side by
        //                                  RendererBackendEGL to communicate with the RendererHostClient on the
        //                                  application process side.
        +[](void* data) -> int { return static_cast<RendererHost*>(data)->addClient().detachIPCChannelPeerFd(); },
        nullptr, nullptr, nullptr, nullptr};

    return &s_interface;
}

RendererHostClient& RendererHost::addClient() noexcept
{
    m_clients.push_back({this});
    return m_clients.back();
}

void RendererHost::removeClient(const RendererHostClient* client) noexcept
{
    if (!client)
        return;

    for (auto it = m_clients.cbegin(); it != m_clients.cend(); ++it)
    {
        if (std::addressof(*it) == client)
        {
            m_clients.erase(it);
            break;
        }
    }
}
