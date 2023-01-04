#include <wpe/webkit.h>
#include <wpebackend-direct.h>

int main(int /*argc*/, const char* /*argv*/[])
{
    g_setenv("WPE_BACKEND_LIBRARY", "libwpebackend-direct.so", TRUE);

    auto* mainLoop = g_main_loop_new(nullptr, FALSE);

    auto* wkManager = webkit_website_data_manager_new_ephemeral();
    webkit_website_data_manager_set_tls_errors_policy(wkManager, WEBKIT_TLS_ERRORS_POLICY_IGNORE);

    auto* wkWebContext = webkit_web_context_new_with_website_data_manager(wkManager);
    g_object_unref(wkManager);

    auto* wkBackend = webkit_web_view_backend_new(wpe_direct_view_backend_create(), nullptr, nullptr);
    auto* wkWebView = webkit_web_view_new_with_context(wkBackend, wkWebContext);
    g_object_unref(wkWebContext);

    auto* settings = webkit_web_view_get_settings(wkWebView);
    webkit_settings_set_enable_webaudio(settings, FALSE);
#ifdef HAS_WEB_SECURITY
    webkit_settings_set_enable_websecurity(settings, FALSE);
#endif // HAS_WEB_SECURITY

    webkit_web_view_load_uri(wkWebView, "https://alteredqualia.com/three/examples/webgl_terrain_dynamic.html");

    g_main_loop_run(mainLoop);

    g_main_loop_unref(mainLoop);
    g_object_unref(wkWebView);
    return 0;
}
