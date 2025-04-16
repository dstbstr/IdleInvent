#pragma once

#include <Platform/Platform.h>
#include <Platform/Graphics.h>

bool Initialize(Platform& platform);
void Run();
#ifdef WIN32
int main(int, char**) {
    auto platform = Platform{nullptr};
    if(!Graphics::Initialize(platform)) {
        return -1;
    }

    if(!Initialize(platform)) {
        return -1;
    }

    Run();
    return 0;
}
#elif defined(__ANDROID__)
#include <android_native_app_glue.h>


void android_main(android_app* app) {
    auto platform = Platform{app};
    /*
    while(platform.GetWindow() == nullptr) {
        platform.HandleInput();
        sleep(10);
    }
        */
    platform.HandleInput();
    if(Graphics::Initialize(platform) && Initialize(platform)) {
        Run();
    }
}
#else
#error "Unsupported platform"
#endif
