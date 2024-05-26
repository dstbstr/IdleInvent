#pragma once

struct Platform {
    Platform(void* app);
    
    // Return a platform specific window handle which must be cast
    void* GetWindow() const;
    int GetAsset(const char* filename, void** outData) const;
    const char* GetInternalDataPath() const;

    // On all platforms?
    void ShowKeyboard() const;
};
