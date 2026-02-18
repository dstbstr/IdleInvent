#pragma once
#include "Platform/DebugStream.h"

#include <optional>
#include <filesystem>

struct Platform {
    Platform(void* app);
    ~Platform();
    std::filesystem::path GetRootPath() const;
    DebugStream& GetDebugStream() const;
    
    // Return a platform specific window handle which must be cast
    void* GetWindow() const;
    std::pair<float, float> GetScreenSize() const;
    bool TryGetAsset(const char* filename, void** outData, size_t& outSize) const;

    //int HandleInput(void* event);
    bool HandleInput();

    // Platform Specifics
#ifdef __ANDROID__
    void ShowKeyboard() const;
#endif
};

void PlatformDebugBreak();
void InitMemoryCheck();
void CheckMemory();

