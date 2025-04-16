#ifdef __ANDROID__

#include "Platform/Platform.h"

#include <android_native_app_glue.h>
#include <backends/imgui_impl_android.h>
#include <imgui.h> // for IM_ALLOC

#include <fstream>

#ifdef DEBUG
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP);
#else
#define DEBUG_BREAK
#endif

namespace {
    
    static void handleAppCmd(android_app* app, int32_t appCmd) {
        switch(appCmd) {
        case APP_CMD_INIT_WINDOW: {
            ANativeWindow_acquire(app->window);
            /*
            platform = std::make_unique<Platform>(app);

            if(app->savedState) {
                saveState = *static_cast<Invent::SaveState*>(app->savedState);
            } else {
                auto loaded = platform->LoadGame();
                if(loaded) {
                    saveState = *loaded;
                } else {
                    saveState = {};
                }
            }
            engine = std::make_unique<Engine>(*platform, saveState);
            */
            break;
        }
        case APP_CMD_TERM_WINDOW: ANativeWindow_release(app->window); break;
        case APP_CMD_SAVE_STATE: {
            /*
            platform->SaveGame(saveState);
            app->savedState = malloc(sizeof(Invent::SaveState));
            app->savedStateSize = sizeof(Invent::SaveState);
            *static_cast<Invent::SaveState*>(app->savedState) = saveState;
            * */
            break;
        }
        default:
            break;
            case APP_CMD_GAINED_FOCUS: // Activity window has gained input focus.
              break;
            case APP_CMD_LOST_FOCUS: // Activity window has lost input focus.
              break;
            case APP_CMD_CONFIG_CHANGED: // Configuration has changed.
              break;
            case APP_CMD_LOW_MEMORY: // System is running low on memory.
              break;
            case APP_CMD_START: // Activity has been started.
              break;
            case APP_CMD_RESUME: // Activity has been resumed.
              break;
            case APP_CMD_PAUSE:
              break;
            case APP_CMD_STOP:
              break;
            case APP_CMD_DESTROY:
              break;
        }
    }
    
} // namespace

void PlatformDebugBreak() {
    DEBUG_BREAK
}
namespace {
    android_app* App;
}

Platform::Platform(void* app) {
    App = static_cast<android_app*>(app);
    App->onAppCmd = handleAppCmd;
    App->onInputEvent = [](android_app*, AInputEvent* e) -> int {
        return ImGui_ImplAndroid_HandleInputEvent(e);
    };
}

Platform::~Platform() { 
    //ANativeWindow_release(App->window); 
}

void* Platform::GetWindow() const {
    return App->window;
}

DebugStream& Platform::GetDebugStream() const {
    static DebugStream stream;
    return stream;
}

bool Platform::HandleInput() { 
    int events;
    android_poll_source* source;
    while(ALooper_pollOnce(App->window == nullptr ? -1 : 0, nullptr, &events, reinterpret_cast<void**>(&source)) >= 0) {
        if(source) source->process(App, source);

        if(App->destroyRequested != 0) {
            return false;
        }
    }

    return true;
}

std::filesystem::path Platform::GetRootPath() const { 
    return std::filesystem::path(App->activity->internalDataPath); 
}
// gather asset from src/main/assets
bool Platform::TryGetAsset(const char* filename, void** outData, size_t& outSize) const {
    GetDebugStream() << "Loading asset: " << filename << std::endl;
    if(AAsset* asset = AAssetManager_open(App->activity->assetManager, filename, AASSET_MODE_BUFFER)) {
        outSize = AAsset_getLength(asset);
        *outData = IM_ALLOC(outSize);
        auto read = static_cast<size_t>(AAsset_read(asset, *outData, outSize));
        AAsset_close(asset);

        GetDebugStream() << std::boolalpha << "Success? " << (read == outSize) << std::endl;
        
        return read == outSize;
    } else {
        GetDebugStream() << "Could not find: " << filename << std::endl;
        IM_ASSERT(false);
        return false;
    }
}

void Platform::ShowKeyboard() const {
    JavaVM* javaVm = App->activity->vm;
    JNIEnv* env = nullptr;

    if(javaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        IM_ASSERT(false);
        return;
    }
    if(javaVm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        IM_ASSERT(false);
        return;
    }

    jclass activityClass = env->GetObjectClass(App->activity->clazz);
    if(activityClass == nullptr) {
        IM_ASSERT(false);
        return;
    }

    jmethodID method = env->GetMethodID(activityClass, "showSoftKeyboard", "()V");
    if(method == nullptr) {
        IM_ASSERT(false);
        return;
    }

    env->CallVoidMethod(App->activity->clazz, method);
    if(javaVm->DetachCurrentThread() != JNI_OK) {
        IM_ASSERT(false);
        return;
    }
}

#endif