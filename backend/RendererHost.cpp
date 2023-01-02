#include "RendererHost.h"

RendererHost& RendererHost::getSingleton() noexcept
{
    static RendererHost s_singleton;
    return s_singleton;
}

wpe_renderer_host_interface* RendererHost::getWPEInterface() noexcept
{
    static wpe_renderer_host_interface s_interface = {
        // void* create() => only stores a singleton on libwpe side, we don't need to use it as we manage our own
        //                   RendererHost singleton here.
        +[]() -> void* { return nullptr; },
        // void destroy(void* data) => never called on libwpe side.
        +[](void*) {},
        // int create_client(void* data) => only called once to fetch the client file descriptor used later from the
        //                                  WPEWebProcess side to communicate with this renderer host on the client
        //                                  process side.
        +[](void*) -> int { return RendererHost::getSingleton().m_ipcServer.dupClientFd(true); }, nullptr, nullptr,
        nullptr, nullptr};

    return &s_interface;
}

void RendererHost::handleMessage(const ipc::Message& /*message*/) noexcept
{
    // Renderer messages received on client process side from the WPEWebProcess side
}
