#pragma once

#include "../common/ipc.h"
#include "../wpebackend-direct.h"

struct wpe_direct_view_backend
{
    // Empty struct used to hide the internal implementation from the public C interface
};

class ViewBackend final : public wpe_direct_view_backend, private IPC::MessageHandler
{
  public:
    static wpe_view_backend_interface* getWPEInterface() noexcept;

    struct ViewParams
    {
        wpe_direct_on_frame_available_callback onFrameAvailableCB;
        void* userData;
        uint32_t width;
        uint32_t height;
    };

    ~ViewBackend() = default;

    ViewBackend(ViewBackend&&) = delete;
    ViewBackend& operator=(ViewBackend&&) = delete;
    ViewBackend(const ViewBackend&) = delete;
    ViewBackend& operator=(const ViewBackend&) = delete;

    wpe_view_backend* getWPEViewBackend() const noexcept
    {
        return m_wpeViewBackend;
    }

    void initialize() noexcept;
    void frameComplete() noexcept;

  private:
    const ViewParams m_viewParams;
    wpe_view_backend* m_wpeViewBackend = nullptr;
    IPC::Channel m_ipcChannel;

    ViewBackend(const ViewParams& viewParams, wpe_view_backend* wpeViewBackend) noexcept
        : m_viewParams(viewParams), m_wpeViewBackend(wpeViewBackend), m_ipcChannel(*this)
    {
    }

    void handleMessage(IPC::Channel& channel, const IPC::Message& message) noexcept override;
};
