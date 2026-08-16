#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstdio>
#include <string>

#define LOG_TAG "TNTTimer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Basic structure of a TNT entity in Bedrock
class Actor {
public:
    virtual ~Actor();
    virtual void normalTick();
    virtual void setNameTag(const std::string& name);
    virtual void setNameTagVisible(bool visible);
};

class PrimedTnt : public Actor {
public:
    int mFuse; // Ticks remaining until explosion (20 ticks = 1 second)
};

// Original tick function pointer
void (*orig_PrimedTnt_normalTick)(PrimedTnt* self) = nullptr;

// Our custom function that runs every single tick for active TNT
void hook_PrimedTnt_normalTick(PrimedTnt* self) {
    if (self) {
        // Calculate remaining seconds
        float remainingSeconds = (float)self->mFuse / 20.0f;

        // Format the text (Yellow if over 1 sec, Red if under 1 sec)
        char timerText[32];
        if (remainingSeconds > 1.0f) {
            snprintf(timerText, sizeof(timerText), "§e%.2fs", remainingSeconds);
        } else {
            snprintf(timerText, sizeof(timerText), "§c%.2fs", remainingSeconds);
        }

        // Show the formatted timer above the TNT block
        self->setNameTag(timerText);
        self->setNameTagVisible(true);
    }

    // Call the original Minecraft tick logic
    if (orig_PrimedTnt_normalTick) {
        orig_PrimedTnt_normalTick(self);
    }
}

// Runs automatically as soon as LeviLauncher loads your plugin
void __attribute__((constructor)) init_plugin() {
    LOGI("TNT Timer Plugin Loaded!");
}
