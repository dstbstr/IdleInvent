#pragma once

#include <string>
#include <fstream>
#include <filesystem>

bool TryWriteFile(std::filesystem::path path, const char* data, int size) {
    auto stream = std::ofstream(path, std::ios::binary | std::ios::out);
    if(!stream.is_open()) {
        return false;
    }
    stream.write(data, size);
    return stream.good();
}

bool TryReadFile(std::filesystem::path path, void* outData, int size) {
    auto stream = std::ifstream(path, std::ios::binary | std::ios::in);
    if(!stream.is_open()) {
        return false;
    }
    stream.read(reinterpret_cast<char*>(outData), size);
    return stream.good();
}

bool TryDeleteFile(const char* path) { 
    return std::remove(path) == 0;
}