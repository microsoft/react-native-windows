# Development Note

## Important

In order to make `REACT_MODULE` cross-platform,
some C++ source files need to be shared between `react-native-windows` repo and `react-native-macos` repo.

Before finishing the effort of porting `REACT_MODULE` to macOS,
some C++ source files are copied from `react-native-windows` to `react-native-macos`.

During this moment,
if anyone want to update these files,
please ensure that the same change to these files must be committed to both
`react-native-windows` repo and `react-native-macos` repo.

It is important to **keep these files in sync between the two repos**.

After finishing the porting work,
files will be shared in some way,
and manual sync will be no longer needed.

## Affected Files

- vnext\Microsoft.ReactNative.Cxx
  - Crash.h
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
- vnext\Microsoft.ReactNative
  - JsiReader.h
  - JsiReader.cpp
  - JsiWriter.h
  - JsiWriter.cpp
  - TurboModulesProvider.h
  - TurboModulesProvider.cpp
- vnext\Shared
  - TurboModuleRegistry
