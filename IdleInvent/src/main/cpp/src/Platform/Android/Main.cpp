#include "Engine.h"

#include <android_native_app_glue.h>

#include <string>
#include <memory>

std::unique_ptr<Engine> engine;
std::unique_ptr<Platform> platform;

static void handleAppCmd(android_app* app, int32_t appCmd) {
    switch(appCmd) {
    case APP_CMD_INIT_WINDOW: platform = std::make_unique<Platform>(app); engine = std::make_unique<Engine>(*platform); break;
    case APP_CMD_TERM_WINDOW: engine.reset(); break;
    default:
        break;
        /*
        case APP_CMD_GAINED_FOCUS:
          break;
        case APP_CMD_LOST_FOCUS:
          break;
        case APP_CMD_CONFIG_CHANGED:
          break;
        case APP_CMD_LOW_MEMORY:
          break;
        case APP_CMD_START:
          break;
        case APP_CMD_RESUME:
          break;
        case APP_CMD_SAVE_STATE:
          break;
        case APP_CMD_PAUSE:
          break;
        case APP_CMD_STOP:
          break;
        case APP_CMD_DESTROY:
          break;
          */
    }
}

void android_main(android_app* app) {
    app->onAppCmd = handleAppCmd;

    while(true) {
        int events;
        android_poll_source* source;
        while(ALooper_pollOnce(engine ? 0 : 1, nullptr, &events, (void**)&source) >= 0) {
            if(source) source->process(app, source);

            if(app->destroyRequested != 0) {
                engine.reset();
                return;
            }
        }

        if(engine) {
          engine->Tick();
        }
    }
}
