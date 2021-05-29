// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactModuleBuilder.h"
#include <strsafe.h>
#include "DynamicWriter.h"
#include "ReactHost/MsoUtils.h"

using namespace facebook::xplat::module;

namespace winrt::Microsoft::ReactNative {

#ifdef DEBUG
// Starting with RNW 0.64, native modules are called on the JS thread.
// Modules will usually call Windows APIs, and those APIs might expect to be called in the UI thread.
// Developers can dispatch work to the UI thread via reactContext.UIDispatcher().Post(). When they fail to do so,
// cppwinrt will grab the failure HRESULT (RPC_E_WRONG_THREAD), convert it to a C++ exception, and throw it.
// However, native module methods are noexcept, meaning the CRT will call std::terminate and not propagate exceptions up
// the stack. Developers are then left with a crashing app and no good way to debug it. To improve developers'
// experience, we can replace the terminate handler temporarily while we call out to the native method. In the terminate
// handler, we can inspect the exception that was thrown and give an error message before going down.
struct TerminateExceptionGuard final {
  TerminateExceptionGuard() {
    m_oldHandler = std::get_terminate();
    std::set_terminate([]() {
      auto ex = std::current_exception();
      if (ex) {
        try {
          std::rethrow_exception(ex);
        } catch (const winrt::hresult_error &hr) {
          wchar_t buf[1024] = {};
          StringCchPrintf(
              buf,
              std::size(buf),
              L"An unhandled exception (0x%x) occurred in a native module. "
              L"The exception message was:\n\n%s",
              hr.code(),
              hr.message().c_str());
          auto messageBox = reinterpret_cast<decltype(&MessageBoxW)>(
              GetProcAddress(GetModuleHandle(L"ext-ms-win-ntuser-dialogbox-l1-1-0.dll"), "MessageBoxW"));
          if (hr.code() == RPC_E_WRONG_THREAD) {
            StringCchCat(
                buf,
                std::size(buf),
                L"\n\nIt's likely that the native module called a Windows API that needs to be called from the UI thread. "
                L"For more information, see https://aka.ms/RNW-UIAPI");
          }
          messageBox(nullptr, buf, L"Unhandled exception in native module", MB_ICONERROR | MB_OK);
        } catch (...) {
        }
      }
    });
  }
  ~TerminateExceptionGuard() {
    std::set_terminate(m_oldHandler);
  }

 private:
  std::terminate_handler m_oldHandler;
};
#define REACT_TERMINATE_GUARD(x) TerminateExceptionGuard x
#else
#define REACT_TERMINATE_GUARD(x)
#endif

//===========================================================================
// ReactModuleBuilder implementation
//===========================================================================

ReactModuleBuilder::ReactModuleBuilder(IReactContext const &reactContext) noexcept : m_reactContext{reactContext} {}

void ReactModuleBuilder::AddInitializer(InitializerDelegate const &initializer) noexcept {
  m_initializers.push_back(initializer);
}

void ReactModuleBuilder::AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept {
  m_constantProviders.push_back(constantProvider);
}

void ReactModuleBuilder::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate const &method) noexcept {
  CxxModule::Method cxxMethod(
      to_string(name), [method](folly::dynamic args, CxxModule::Callback resolve, CxxModule::Callback reject) noexcept {
        auto argReader = make<DynamicReader>(args);
        auto resultWriter = make<DynamicWriter>();
        auto resolveCallback = MakeMethodResultCallback(std::move(resolve));
        auto rejectCallback = MakeMethodResultCallback(std::move(reject));

        REACT_TERMINATE_GUARD(term);

        method(argReader, resultWriter, resolveCallback, rejectCallback);
      });

  switch (returnType) {
    case MethodReturnType::Callback:
      cxxMethod.callbacks = 1;
      cxxMethod.isPromise = false;
      break;
    case MethodReturnType::TwoCallbacks:
      cxxMethod.callbacks = 2;
      cxxMethod.isPromise = false;
      break;
    case MethodReturnType::Promise:
      cxxMethod.callbacks = 2;
      cxxMethod.isPromise = true;
      break;
    default:
      cxxMethod.callbacks = 0;
      cxxMethod.isPromise = false;
  }

  m_methods.push_back(std::move(cxxMethod));
}

void ReactModuleBuilder::AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
  CxxModule::Method cxxMethod(
      to_string(name),
      [method](folly::dynamic args) noexcept {
        auto argReader = make<DynamicReader>(args);
        auto resultWriter = make<DynamicWriter>();
        method(argReader, resultWriter);
        return get_self<DynamicWriter>(resultWriter)->TakeValue();
      },
      CxxModule::SyncTag);

  m_methods.push_back(std::move(cxxMethod));
}

/*static*/ MethodResultCallback ReactModuleBuilder::MakeMethodResultCallback(CxxModule::Callback &&callback) noexcept {
  if (callback) {
    return [callback = std::move(callback)](const IJSValueWriter &outputWriter) noexcept {
      if (outputWriter) {
        folly::dynamic argArray = outputWriter.as<DynamicWriter>()->TakeValue();
        callback(std::vector<folly::dynamic>(argArray.begin(), argArray.end()));
      } else {
        callback(std::vector<folly::dynamic>{});
      }
    };
  }

  return {};
}

std::unique_ptr<CxxModule> ReactModuleBuilder::MakeCxxModule(
    std::string const &name,
    IInspectable const &nativeModule) noexcept {
  for (auto &initializer : m_initializers) {
    initializer(m_reactContext);
  }
  return std::make_unique<ABICxxModule>(
      nativeModule, Mso::Copy(name), Mso::Copy(m_constantProviders), Mso::Copy(m_methods));
}

} // namespace winrt::Microsoft::ReactNative
