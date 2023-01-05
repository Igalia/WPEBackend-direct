#include "ViewBackend.h"

#include "../common/ipc-messages.h"

wpe_view_backend_interface* ViewBackend::getWPEInterface() noexcept
{
    static wpe_view_backend_interface s_interface = {
        // void* create(void* params, wpe_view_backend* backend)
        +[](void* params, wpe_view_backend* backend) -> void* {
            auto* viewParams = static_cast<ViewBackend::ViewParams*>(params);
            auto* viewBackend = new ViewBackend(*viewParams, backend);
            viewParams->userData = viewBackend;
            return viewBackend;
        },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<ViewBackend*>(data); },
        // void initialize(void* data)
        +[](void* data) { static_cast<ViewBackend*>(data)->initialize(); },
        // int get_renderer_host_fd(void* data)
        +[](void* data) -> int { return static_cast<ViewBackend*>(data)->m_ipcChannel.detachPeerFd(); }, nullptr,
        nullptr, nullptr, nullptr};

    return &s_interface;
}

void ViewBackend::initialize() noexcept
{
    wpe_view_backend_dispatch_set_size(m_wpeViewBackend, m_viewParams.width, m_viewParams.height);
}

void ViewBackend::frameComplete() noexcept
{
    m_ipcChannel.sendMessage(IPC::FrameComplete());
    wpe_view_backend_dispatch_frame_displayed(m_wpeViewBackend);
}

void ViewBackend::handleMessage(IPC::Channel& /*channel*/, const IPC::Message& message) noexcept
{
    // Messages received on application process side from RendererBackendEGLTarget on WPEWebProcess side
    switch (message.getCode())
    {
    case IPC::FrameAvailable::MESSAGE_CODE:
        if (m_viewParams.onFrameAvailableCB)
            m_viewParams.onFrameAvailableCB(this, m_viewParams.userData);
        else
            frameComplete();
        break;

    default:
        break;
    }
}
