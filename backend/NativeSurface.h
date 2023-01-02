#pragma once

#include <epoxy/egl.h>
#include <memory>

class NativeSurface
{
  public:
    static std::unique_ptr<NativeSurface> createNativeSurface(unsigned int width, unsigned int height) noexcept;
    virtual ~NativeSurface() = default;

    NativeSurface(NativeSurface&&) = delete;
    NativeSurface& operator=(NativeSurface&&) = delete;
    NativeSurface(const NativeSurface&) = delete;
    NativeSurface& operator=(const NativeSurface&) = delete;

    unsigned int getWidth() const noexcept
    {
        return m_width;
    }

    unsigned int getHeight() const noexcept
    {
        return m_height;
    }

    EGLNativeDisplayType getDisplay() const noexcept
    {
        return m_display;
    }

    EGLNativeWindowType getWindow() const noexcept
    {
        return m_window;
    }

    virtual EGLenum getPlatform() const noexcept = 0;

    void resize(unsigned int width, unsigned int height) noexcept;

  protected:
    NativeSurface(unsigned int width, unsigned int height);

    unsigned int m_width = 1;
    unsigned int m_height = 1;

    EGLNativeDisplayType m_display = nullptr;
    EGLNativeWindowType m_window = nullptr;

    virtual void resizeUnderlyingWindow() noexcept = 0;
};
