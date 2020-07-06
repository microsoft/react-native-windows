#include "pch.h"

#include "TestController.h"

#include "Microsoft.Internal.TestController.g.cpp"

namespace winrt::Microsoft::Internal::implementation {

hstring TestController::HelloWorld() {
  return L"Hello World!";
}

} // namespace winrt::Microsoft::Internal::implementation
