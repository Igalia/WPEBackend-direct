#include "ViewBackend.h"

wpe_view_backend_interface* ViewBackend::getWPEInterface() noexcept
{
    static wpe_view_backend_interface s_interface = {
        // void* create(void* userData, wpe_view_backend* backend)
        +[](void*, wpe_view_backend* backend) -> void* { return new ViewBackend(backend); },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<ViewBackend*>(data); },
        // void initialize(void* data)
        +[](void* data) { static_cast<ViewBackend*>(data)->initialize(); },
        // int get_renderer_host_fd(void* data)
        +[](void* data) -> int { return static_cast<ViewBackend*>(data)->m_ipcChannel.detachPeerFd(); }, nullptr,
        nullptr, nullptr, nullptr};

    return &s_interface;
}

void ViewBackend::initialize()
{
}

void ViewBackend::handleMessage(IPC::Channel& /*channel*/, const IPC::Message& /*message*/) noexcept
{
    // Messages received on application process side from RendererBackendEGLTarget on WPEWebProcess side
}
