#pragma once

#include "RendererHostClient.h"

#include <vector>
#include <wpe/wpe.h>

class RendererHost final
{
  public:
    static wpe_renderer_host_interface* getWPEInterface() noexcept;

    ~RendererHost() = default;

    RendererHost(RendererHost&&) = delete;
    RendererHost& operator=(RendererHost&&) = delete;
    RendererHost(const RendererHost&) = delete;
    RendererHost& operator=(const RendererHost&) = delete;

    RendererHostClient& addClient() noexcept;
    void removeClient(const RendererHostClient* client) noexcept;

  private:
    RendererHost() = default;
    std::vector<RendererHostClient> m_clients;
};
