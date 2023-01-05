#include "RendererBackendEGLTarget.h"

#include "../common/ipc-messages.h"

wpe_renderer_backend_egl_target_interface* RendererBackendEGLTarget::getWPEInterface() noexcept
{
    static wpe_renderer_backend_egl_target_interface s_interface = {
        // void* create(wpe_renderer_backend_egl_target* target, int peerFd)
        +[](wpe_renderer_backend_egl_target* target, int peerFd) -> void* {
            return new RendererBackendEGLTarget(target, peerFd);
        },
        // void destroy(void* data)
        +[](void* data) { delete static_cast<RendererBackendEGLTarget*>(data); },
        // void initialize(void* targetData, void* backendData, uint32_t width, uint32_t height)
        +[](void* targetData, void* backendData, uint32_t width, uint32_t height) {
            static_cast<RendererBackendEGLTarget*>(targetData)
                ->init(static_cast<RendererBackendEGL*>(backendData), width, height);
        },
        // EGLNativeWindowType get_native_window(void* data)
        +[](void* data) -> EGLNativeWindowType {
            return static_cast<RendererBackendEGLTarget*>(data)->getNativeWindow();
        },
        // void resize(void* data, uint32_t width, uint32_t height)
        +[](void* data, uint32_t width, uint32_t height) {
            static_cast<RendererBackendEGLTarget*>(data)->resize(width, height);
        },
        // void frame_will_render(void* data)
        +[](void* data) { static_cast<RendererBackendEGLTarget*>(data)->frameWillRender(); },
        // void frame_rendered(void* data)
        +[](void* data) { static_cast<RendererBackendEGLTarget*>(data)->frameRendered(); },
        // void deinitialize(void* data)
        +[](void* data) { static_cast<RendererBackendEGLTarget*>(data)->shut(); }, nullptr, nullptr, nullptr};

    return &s_interface;
}

void RendererBackendEGLTarget::init(RendererBackendEGL* backend, uint32_t width, uint32_t height) noexcept
{
    if (m_backend || m_nativeSurface)
    {
        g_warning("RendererBackendEGLTarget is already initialized");
        return;
    }

    m_backend = backend;
    m_nativeSurface =
        NativeSurface::createNativeSurface(m_backend->getPlatform(), m_backend->getDisplay(), width, height);
    if (!m_nativeSurface)
    {
        g_critical("Cannot create RendererBackendEGLTarget native surface");
        shut();
    }
}

void RendererBackendEGLTarget::shut() noexcept
{
    m_backend = nullptr;
    m_nativeSurface = nullptr;
}

void RendererBackendEGLTarget::frameWillRender() const noexcept
{
    // Frame drawing started in ThreadedCompositor::renderLayerTree() from WPEWebProcess
}

void RendererBackendEGLTarget::frameRendered() const noexcept
{
    // Frame drawing finished (and buffers swapped) in ThreadedCompositor::renderLayerTree() from WPEWebProcess
    m_ipcChannel.sendMessage(IPC::FrameAvailable());
}

void RendererBackendEGLTarget::handleMessage(IPC::Channel& /*channel*/, const IPC::Message& message) noexcept
{
    // Messages received on WPEWebProcess side from ViewBackend on application process side
    switch (message.getCode())
    {
    case IPC::FrameComplete::MESSAGE_CODE:
        wpe_renderer_backend_egl_target_dispatch_frame_complete(m_wpeTarget);
        break;

    default:
        break;
    }
}
