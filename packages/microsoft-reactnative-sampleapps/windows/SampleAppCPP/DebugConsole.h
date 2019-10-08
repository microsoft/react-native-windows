#pragma once

#include <functional>
#include "NativeModules.h"

namespace TestApp {

REACT_MODULE(DebugConsole);
struct DebugConsole {
  REACT_METHOD(Log);
  void Log(std::string message) noexcept {
    OutputDebugStringA(message.c_str());
  }
};

} // namespace TestApp
