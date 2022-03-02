#include "pch.h"
#include "Exports.h"
#include <winrt/Microsoft.ReactNative.h>
namespace react = winrt::Microsoft::ReactNative;

extern "C" void *RNStartCoreApp(void (*callback)(RNCoreApp *)) {
  void *abi{nullptr};
  xaml::Application::Start([&abi, callback](auto &&) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    auto app = react::CoreApp([callback](react::CoreApp const &sender) {
      RNCoreApp rnca{winrt::get_abi(sender)};
      if (callback) {
        callback(&rnca);
      }
      sender.JavaScriptBundleFile(rnca.jsBundleFile);
      auto settings = sender.InstanceSettings();
      settings.UseWebDebugger(rnca.useWebDebugger);
      settings.UseFastRefresh(rnca.useFastRefresh);
      settings.UseDeveloperSupport(rnca.useDeveloperSupport);
    });
    abi = winrt::get_abi(app);
  });

  return abi;
}
