#include "Engine.h"

#include "InventLib/GameState/SaveState.h"

#include <android_native_app_glue.h>

#include <string>
#include <memory>

std::unique_ptr<Engine> engine;
std::unique_ptr<Platform> platform;
Invent::SaveState saveState{};


static void handleAppCmd(android_app* app, int32_t appCmd) {
    switch(appCmd) {
    case APP_CMD_INIT_WINDOW: {
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
      break;
    }
    case APP_CMD_TERM_WINDOW: engine.reset(); break;
    case APP_CMD_SAVE_STATE: {
      platform->SaveGame(saveState);
      app->savedState = malloc(sizeof(Invent::SaveState));
      app->savedStateSize = sizeof(Invent::SaveState);
      *static_cast<Invent::SaveState*>(app->savedState) = saveState;
      break;
    }
    default:
        break;
        /*
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
        case APP_CMD_SAVE_STATE: // App needs to save, and will be restored later (save in android_app.savzedState)
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
