#pragma once

#include "NativeSurface.h"
#include "Renderer.h"

class Backend final
{
  public:
    static std::unique_ptr<Backend> createBackend(unsigned int width, unsigned int height) noexcept;
    ~Backend() = default;

    Backend(Backend&&) = delete;
    Backend& operator=(Backend&&) = delete;
    Backend(const Backend&) = delete;
    Backend& operator=(const Backend&) = delete;

    EGLNativeDisplayType getDisplay() const noexcept
    {
        return m_nativeSurface->getDisplay();
    }

    EGLenum getPlatform() const noexcept
    {
        return m_nativeSurface->getPlatform();
    }

    EGLNativeWindowType getWindow() const noexcept
    {
        return m_nativeSurface->getWindow();
    }

    void frameWillRender() const noexcept;
    void frameRendered() const noexcept;

  private:
    Backend() = default;

    std::unique_ptr<NativeSurface> m_nativeSurface;
    std::unique_ptr<Renderer> m_renderer;
};
