#include "Backend.h"

std::unique_ptr<Backend> Backend::createBackend(unsigned int width, unsigned int height) noexcept
{
    std::unique_ptr<Backend> backend(new Backend());

    backend->m_nativeSurface = NativeSurface::createNativeSurface(width, height);
    if (!backend->m_nativeSurface)
        return nullptr;

    backend->m_renderer =
        Renderer::createRenderer(backend->m_nativeSurface->getDisplay(), backend->m_nativeSurface->getWindow());
    if (!backend->m_renderer)
        return nullptr;

    return backend;
}

void Backend::frameWillRender() const noexcept
{
    // TODO
}

void Backend::frameRendered() const noexcept
{
    // TODO
    glFlush();
}
