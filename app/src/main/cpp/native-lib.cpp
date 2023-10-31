#include <jni.h>
#include <string>
#include <android/log.h>
#include "frida-gumjs.h"

#define TAG "BHGUM" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

static void on_message (const gchar * message, GBytes * data, gpointer user_data);

static void
on_message (const gchar * message,
            GBytes * data,
            gpointer user_data)
{
    JsonParser * parser;
    JsonObject * root;
    const gchar * type;

    parser = json_parser_new ();
    json_parser_load_from_data (parser, message, -1, NULL);
    root = json_node_get_object (json_parser_get_root (parser));

    type = json_object_get_string_member (root, "type");
    if (strcmp (type, "log") == 0)
    {
        const gchar * log_message;

        log_message = json_object_get_string_member (root, "payload");
        g_print ("%s\n", log_message);
    }
    else
    {
        g_print ("on_message: %s\n", message);
    }

    g_object_unref (parser);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    //LOGD("my name is JNI_OnLoad");
    if (jvm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    result = JNI_VERSION_1_4;
    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_android_util_BHHook_loadJs(JNIEnv *env, jclass clazz, jstring js_content) {
    // TODO: implement loadJs()
    GumScriptBackend * backend;
    GCancellable * cancellable = NULL;
    GError * error = NULL;
    GumScript * script;
    GBytes  *snapshot =NULL;
    GMainContext * context;
    //LOGD("Java_android_util_BHHook_loadJs");
    gum_init_embedded ();
    // 选择js引擎后端，还可选v8
    backend = gum_script_backend_obtain_qjs ();
    const char* jsContent = env->GetStringUTFChars(js_content, 0);
    // 创建桩代码
    script = gum_script_backend_create_sync (backend, "BHGUM",jsContent,NULL, cancellable, &error);
    g_assert (error == NULL);

    // 指定message回调函数,加上这句话报错
    //gum_script_set_message_handler (script, on_message, NULL, NULL);

    // 加载脚本
    gum_script_load_sync (script, cancellable);

    context = g_main_context_get_thread_default ();
    while (g_main_context_pending (context))
        g_main_context_iteration (context, FALSE);

    // 结束清理现场
   // gum_script_unload_sync (script, cancellable);

   // g_object_unref (script);

   // gum_deinit_embedded ();
   // env->ReleaseStringUTFChars(js_content, jsContent);
}