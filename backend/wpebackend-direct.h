#pragma once

#include <wpe/wpe.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct wpe_direct_view_backend;
    typedef void (*wpe_direct_on_frame_available_callback)(struct wpe_direct_view_backend* direct_backend,
                                                           void* user_data);

    // The returned wpe_direct_view_backend pointer is also stored into the interface_data field of the
    // associated wpe_view_backend_base, so it is automatically destroyed when calling wpe_view_backend_destroy.
    struct wpe_direct_view_backend* wpe_direct_view_backend_create(wpe_direct_on_frame_available_callback cb,
                                                                   void* user_data, uint32_t width, uint32_t height);

    struct wpe_view_backend* wpe_direct_view_backend_get_wpe_backend(struct wpe_direct_view_backend* direct_backend);
    void wpe_direct_view_backend_dispatch_frame_complete(struct wpe_direct_view_backend* direct_backend);

#ifdef __cplusplus
}
#endif
