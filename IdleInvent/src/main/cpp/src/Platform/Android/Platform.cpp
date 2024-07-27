#ifdef __ANDROID__

#include "Platform/Platform.h"

#include "Core/Instrumentation/Logging.h"
#include "Platform/Inputs.h"

#include <android_native_app_glue.h>
#include <imgui.h> // for IM_ALLOC

#include <fstream>

namespace {
    android_app* App;
    std::string GetPath(const Platform& platform) {
        return std::string(platform.GetInternalDataPath()) + "/save.dat";
    }
}

Platform::Platform(void* app) {
    App = static_cast<android_app*>(app);
    App->onInputEvent = [](android_app*, AInputEvent* e) -> int {
        return Inputs::HandleInput(static_cast<void*>(e));
    };
}

void* Platform::GetWindow() const {
    return App->window;
}

const char* Platform::GetInternalDataPath() const {
    return App->activity->internalDataPath;
}

// gather asset from src/main/assets
int Platform::GetAsset(const char* filename, void** outData) const {
    Log::Info(std::format("Loading asset: {}", filename));
    int size = 0;
    if(AAsset* asset = AAssetManager_open(App->activity->assetManager, filename, AASSET_MODE_BUFFER)) {
        size = AAsset_getLength(asset);
        *outData = IM_ALLOC(size);
        int64_t read = AAsset_read(asset, *outData, size);
        AAsset_close(asset);

        DR_ASSERT_MSG(read == size, "Read size mismatch");
    } else {
        Log::Warn("GetAssetData failed");
    }

    return size;
}

void Platform::ShowKeyboard() const {
    JavaVM* javaVm = App->activity->vm;
    JNIEnv* env = nullptr;

    DR_ASSERT_MSG(javaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK, "GetEnv failed");
    DR_ASSERT_MSG(javaVm->AttachCurrentThread(&env, nullptr) == JNI_OK, "AttachCurrentThread Failed");

    jclass activityClass = env->GetObjectClass(App->activity->clazz);
    DR_ASSERT_MSG(activityClass != nullptr, "GetObjectClass failed");

    jmethodID method = env->GetMethodID(activityClass, "showSoftKeyboard", "()V");
    DR_ASSERT_MSG(method != nullptr, "GetMethod showSoftKeyboard failed");

    env->CallVoidMethod(App->activity->clazz, method);
    DR_ASSERT_MSG(javaVm->DetachCurrentThread() == JNI_OK, "DetachCurrentThread failed");
}

void Platform::SaveGame(const Invent::SaveState& saveState) const {
    auto stream = std::ofstream(GetPath(*this), std::ios::binary | std::ios::out);
    stream.write(reinterpret_cast<const char*>(&saveState), sizeof(Invent::SaveState));
}

std::optional<Invent::SaveState> Platform::LoadGame() const {
    auto stream = std::ifstream(GetPath(*this), std::ios::binary | std::ios::in);
    if(stream.is_open()) {
        Invent::SaveState saveState;
        stream.read(reinterpret_cast<char*>(&saveState), sizeof(Invent::SaveState));
        return saveState;
    }
    return std::nullopt;
}

void Platform::DeleteGame() const {
    std::remove(GetPath(*this).c_str());
}

#endif