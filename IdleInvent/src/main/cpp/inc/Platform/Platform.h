#pragma once

#include "InventLib/GameState/SaveState.h"

#include <optional>

struct Platform {
    Platform(void* app);
    
    // Return a platform specific window handle which must be cast
    void* GetWindow() const;
    int GetAsset(const char* filename, void** outData) const;
    const char* GetInternalDataPath() const;

    // On all platforms?
    void ShowKeyboard() const;

    void SaveGame(const Invent::SaveState& save) const;
    std::optional<Invent::SaveState> LoadGame() const;
    void DeleteGame() const;
};
