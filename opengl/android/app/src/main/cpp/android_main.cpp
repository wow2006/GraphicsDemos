// Internal
#include "android_native_app_glue.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

struct Engine final {
  bool g_bAnimating = false;
  android_app *m_pApplication = nullptr;
};

static Engine gEngine;

static void handleCommand(android_app* /*pApp*/, int32_t command) {
  switch(command) {
    case APP_CMD_SAVE_STATE:
      LOGI("APP_CMD_SAVE_STATE");
      break;
    case APP_CMD_INIT_WINDOW:
      LOGI("APP_CMD_INIT_WINDOW");
      break;
    case APP_CMD_TERM_WINDOW:
      LOGI("APP_CMD_TERM_WINDOW");
      break;
    case APP_CMD_GAINED_FOCUS:
      LOGI("APP_CMD_GAINED_FOCUS");
      break;
    case APP_CMD_LOST_FOCUS:
      LOGI("APP_CMD_LOST_FOCUS");
      break;
    default:
      LOGI("default");
      break;
  }
}

static int32_t handleInput(android_app* /*pApp*/, AInputEvent* pEvent) {
  if(AInputEvent_getType(pEvent) == AINPUT_EVENT_TYPE_MOTION) {
    gEngine.g_bAnimating = true;
    return 1;
  }
  return 0;
}

static void drawFrame() {}

static void cleanUp() {}

[[maybe_unused]] void android_main(android_app *pState) {
  pState->onAppCmd     = handleCommand;
  pState->onInputEvent = handleInput;

  bool bRunning = true;
  while(bRunning) {
    int ident  = 0;
    int events = 0;
    android_poll_source *pSource = nullptr;

    while((ident = ALooper_pollAll(-1, nullptr, &events,
                                   reinterpret_cast<void **>(&pSource))) >= 0) {
      if(pSource != nullptr) {
        pSource->process(pState, pSource);
      }

      if(pState->destroyRequested != 0) {
        bRunning = false;
      }

      if(pState->destroyRequested != 0) {
        cleanUp();
        return;
      }
    }

    if(gEngine.g_bAnimating) {
      drawFrame();
    }
  }
}

