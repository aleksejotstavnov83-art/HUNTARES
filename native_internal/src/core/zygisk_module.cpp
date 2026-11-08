#include "zygisk.hpp"
#include <cstring>
#include <android/log.h>

extern "C" void HuntaresInternalStart(JavaVM* vm);

class HuntaresZygiskModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api* api, JNIEnv* env) override {
        api_ = api;
        env_ = env;
    }
    void preAppSpecialize(zygisk::AppSpecializeArgs* args) override {
        target_ = false;
        if (env_ && args && args->nice_name) {
            const char* name = env_->GetStringUTFChars(args->nice_name, nullptr);
            if (name) {
                target_ = std::strcmp(name, "com.axlebolt.standoff2") == 0;
                env_->ReleaseStringUTFChars(args->nice_name, name);
            }
        }
        if (!target_ && api_) api_->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
    }
    void postAppSpecialize(const zygisk::AppSpecializeArgs*) override {
        if (!target_ || !env_) return;
        JavaVM* vm = nullptr;
        if (env_->GetJavaVM(&vm) == JNI_OK) {
            __android_log_print(ANDROID_LOG_INFO, "HUNTARES_INTERNAL", "Target process loaded by Zygisk");
            HuntaresInternalStart(vm);
        }
    }
private:
    zygisk::Api* api_ = nullptr;
    JNIEnv* env_ = nullptr;
    bool target_ = false;
};
REGISTER_ZYGISK_MODULE(HuntaresZygiskModule)
