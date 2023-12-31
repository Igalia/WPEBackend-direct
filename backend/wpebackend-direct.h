/*
 * Copyright (C) 2023 Igalia S.L.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
