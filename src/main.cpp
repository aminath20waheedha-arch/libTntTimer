#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstdio>
#include <string>

#define LOG_TAG "TNTTimerDebug"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class Actor {
public:
    virtual ~Actor();
    virtual void normalTick();
    virtual void setNameTag(const std::string& name);
    virtual void setNameTagVisible(bool visible);
};

class PrimedTnt : public Actor {
public:
    int mFuse;
};

void (*orig_PrimedTnt_normalTick)(PrimedTnt* self) = nullptr;

void hook_PrimedTnt_normalTick(PrimedTnt* self) {
    if (self) {
        // Log every tick execution to confirm the hook is active
        LOGI("[DEBUG] PrimedTnt::normalTick called! Fuse remaining: %d ticks", self->mFuse);

        float remainingSeconds = (float)self->mFuse / 20.0f;

        char timerText[32];
        if (remainingSeconds > 1.0f) {
            snprintf(timerText, sizeof(timerText), "§e%.2fs", remainingSeconds);
        } else {
            snprintf(timerText, sizeof(timerText), "§c%.2fs", remainingSeconds);
        }

        self->setNameTag(timerText);
        self->setNameTagVisible(true);
    } else {
        LOGE("[DEBUG] PrimedTnt instance pointer is null!");
    }

    if (orig_PrimedTnt_normalTick) {
        orig_PrimedTnt_normalTick(self);
    }
}

// Runs when LeviLauncher loads the .so library into memory
void __attribute__((constructor)) init_plugin() {
    LOGI("=====================================");
    LOGI("[DEBUG] TNT Timer Plugin constructor triggered!");

    void* mc_lib = dlopen("libminecraftpe.so", RTLD_LAZY);
    if (!mc_lib) {
        LOGE("[DEBUG] Failed to load libminecraftpe.so: %s", dlerror());
        return;
    }
    LOGI("[DEBUG] Successfully acquired handle to libminecraftpe.so");

    // Attempting symbol resolution
    void* tntTickSym = dlsym(mc_lib, "_ZN9PrimedTnt10normalTickEv");
    if (tntTickSym) {
        LOGI("[DEBUG] Found symbol _ZN9PrimedTnt10normalTickEv at address: %p", tntTickSym);
    } else {
        LOGE("[DEBUG] Symbol _ZN9PrimedTnt10normalTickEv NOT found! Offset hooking may be required.");
    }
    LOGI("=====================================");
}
