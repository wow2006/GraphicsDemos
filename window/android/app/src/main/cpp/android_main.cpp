// Internal
#include "android_native_app_glue.h"

static void HandleCMD(android_app* /*pApp*/, int32_t /*cmd*/) {}

static int32_t HandleInput(android_app* /*pApp*/, AInputEvent* /*pEvent*/) { return 0; }

[[maybe_unused]] void android_main(android_app *pState) {
  pState->onAppCmd     = HandleCMD;
  pState->onInputEvent = HandleInput;

  auto bRunning = true;
  while(bRunning) {
    int ident;
    int events;
    android_poll_source *pSource = nullptr;

    while((ident = ALooper_pollAll(-1, nullptr, &events, reinterpret_cast<void **>(&pSource))) >= 0) {
      if(pSource != nullptr) {
        pSource->process(pState, pSource);
      }

      if(pState->destroyRequested != 0) {
        bRunning = false;
      }
    }
  }
}

