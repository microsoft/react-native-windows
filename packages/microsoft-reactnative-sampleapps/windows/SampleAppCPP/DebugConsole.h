#pragma once

#include <functional>
#include "NativeModules.h"

namespace TestApp {

struct DebugConsole {

  REACT_METHOD(Log);
  void Log(std::string message) noexcept {
    OutputDebugStringA(message.c_str());
  }
};

} // namespace TestApp
