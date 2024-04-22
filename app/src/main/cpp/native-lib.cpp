#include <jni.h>
#include <string>
#include <android/log.h>
#include "frida-gumjs.h"
#include "decryption.h"
#include "json.hpp"

#define TAG "BHGUM"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__)

using json = nlohmann::json;

static void on_message (const gchar * message, GBytes *, gpointer)
{
    try {
        json msg = json::parse(message);
        if (msg.is_object() && msg.contains("type")) {
            std::string type = msg["type"].get<std::string>();
            if (type == "error" && msg.contains("description")) {
                std::string desc = msg["description"].get<std::string>();
                LOGE("%s", desc.c_str());
            } else if (type == "log" && msg.contains("payload")) {
                std::string payload = msg["payload"].get<std::string>();
                LOGI("%s", payload.c_str());
            } else {
                LOGW("%s", message);
            }
        } else {
            LOGE("%s", message);
        }
    } catch (const nlohmann::json::parse_error& ex) {
        LOGW("%s", message);
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    if (jvm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    result = JNI_VERSION_1_4;
    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_android_util_BHHook_loadJs(JNIEnv *env, jclass clazz, jstring content) {
//JJava_com_dorry_gumjs_BHHook_loadJs(JNIEnv *env, jclass clazz, jstring content) {
    GumScriptBackend * backend;
    GCancellable * cancellable = NULL;
    GError * error = NULL;
    GumScript * script;
    GMainContext * context;
    const char* jsContent = env->GetStringUTFChars(content, 0);
    if (jsContent == nullptr) {
        LOGE("js content is null");
        return;
    }

    gum_init_embedded();
    backend = gum_script_backend_obtain_qjs ();
    std::string js(jsContent);
    auto k1 = js.find("Interceptor.");
    auto k2 = js.find("Java.perform");
    if (!(k1 != std::string::npos || k2 != std::string::npos)) {
        js_decr(jsContent, js.size(), js);
    }

    script = gum_script_backend_create_sync (backend, TAG,js.c_str(),NULL, cancellable, &error);
    if (error != nullptr) {
        LOGW("load failed, code:%d msg%s", error->code, error->message);
        return;
    }

    gum_script_set_message_handler (script, on_message, NULL, NULL);
    gum_script_load_sync (script, cancellable);
    context = g_main_context_get_thread_default ();
    while (g_main_context_pending (context)) {
        g_main_context_iteration (context, FALSE);
    }
    LOGI("loaded success");
    // gum_script_unload_sync (script, cancellable);
    // g_object_unref (script);
    // gum_deinit_embedded ();
    // env->ReleaseStringUTFChars(js_content, jsContent);
}