# Development Note

## Important

In order to make `REACT_MODULE` cross-platform,
some C++ source files need to be shared between `react-native-windows` repo and `react-native-macos` repo.

Whenever:
- new files need to be shared

Please add this comment as in other files:
```C++
// IMPORTANT: Before updating this file
// please read react-native-windows repo:
// packages/react-native-windows/Microsoft.ReactNative.Cxx/README.md
```

Whenever:
- files are added to the following folders
- listed files are edited

You are required to create a pull request in `react-native-macos` to prove that your change is compatible with macOS by doing:

- Update this file: https://github.com/microsoft/react-native-macos/blob/master/ReactTurboModuleCxx/React-TurboModuleCxx-RNW.podspec
  - Change `s.source` to your own fork and commit
  - Follow the instruction to build and test `RNTester/RNTester-macOS` by running the `Snapshot/Screenshot` test page
  - Create a PR in `react-native-macos` to show the result
  - In some cases, you will also need to update files in https://github.com/microsoft/react-native-macos/tree/master/ReactTurboModuleCxx to make your change work in macOS
- Submit your change to `react-native-windows`
- Update the podspec file again with `s.source` pointing to the commit that is just submitted to `react-native-windows`
- Submit your change to `react-native-macos`

## Affected Files

- packages\react-native-windows\Microsoft.ReactNative.Cxx
  - StructInfo.h
  - JSValue.h
  - JSValue.cpp
  - JSValueTreeReader.h
  - JSValueTreeReader.cpp
  - JSValueTreeWriter.h
  - JSValueTreeWriter.cpp
  - JSValueReader.h
  - JSValueWriter.h
  - ModuleRegistration.h
  - ModuleRegistration.cpp
  - ReactPropertyBag.h
  - ReactHandleHelper.h
  - ReactNonAbiValue.h
  - ReactDispatcher.h
  - ReactNotificationService.h
  - ReactContext.h
  - ReactError.h
  - ReactPromise.h
  - ReactPromise.cpp
  - NativeModules.h
- packages\react-native-windows\Microsoft.ReactNative
  - JsiReader.h
  - JsiReader.cpp
  - JsiWriter.h
  - JsiWriter.cpp
  - TurboModulesProvider.h
  - TurboModulesProvider.cpp
- packages\react-native-windows\Shared
  - TurboModuleRegistry
