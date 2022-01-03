# React Native Turbo Module C++ Code Generation

## Running Codegen

(working)

## TypeScript Turbo Module Syntax

(working)

## Implementing a Turbo Module in C++

Checkout the following example:

- Flow definition: [Alert.windows.js](https://github.com/microsoft/react-native-windows/blob/main/vnext/src/Libraries/Alert/Alert.windows.js)
- Generated spec file: [NativeDialogManagerWindowsSpec.g.h](https://github.com/microsoft/react-native-windows/blob/main/vnext/codegen/NativeDialogManagerWindowsSpec.g.h)
- Implementation: [AlertModule.h](https://github.com/microsoft/react-native-windows/blob/main/vnext/Microsoft.ReactNative/Modules/AlertModule.h)

A turbo module implemetation starts with:

```c++
#include <MyModuleSpec.g.h>
#include <NativeModules.h>

namespace MyNamespace
{
    REACT_MODULE(MyModule)
    struct MyModule
    {
        using ModuleSpec = MyModuleSpec;
    };
}
```

You could call `winrt::Microsoft::ReactNative::MakeModuleProvider<MyNamespace::MyModule>`
or `winrt::Microsoft::ReactNative::MakeTurboModuleProvider<MyNamespace::MyModule>`
to create a `winrt::Microsoft::ReactNative::ReactModuleProvider` object and register it later.

By calling `MakeModuleProvider<MyModule>`,
if there is `using ModuleSpec = MyModuleSpec;` in `MyModule`,
the compiler will compare `MyModule` with `MyModuleSpec` and see if all required members are defined.
Failed to define any required member results in compiler errors telling you what member is needed.
All acceptable member declarations in C++ for a missing member will be included in error messages,
you could pick one and copy to your `MyModule`.

`MakeTurboModuleProvider<MyModule>` does exactly what `MakeModuleProvider<MyModule>` does,
but it requires `MyModule::ModuleSpec` to exist,
unlike `MakeModuleProvider<MyModule>` where `MyModule::ModuleSpec` is optional.

### REACT_MODULE

The syntax of `REACT_MODULE` is `REACT_MODULE(moduleStruct, [optional]moduleName, [optional]eventEmitterName)`.

- `moduleStruct`: the struct name that implements the turbo module without namespace.
- `moduleName`: the module name visible to JavaScript. The default value is `L"moduleStruct"`.
- `eventEmitterName`: the default event emitter name. The default value is `L""`, which is `RCTDeviceEventEmitter`.

### Members

Any member in the struct implementing turbo module functions should be attached with an attribute like `REACT_MODULE`.
Here is the list of all available attributes:

- `REACT_INIT`
- `REACT_METHOD`
- `REACT_SYNC_METHOD`
- `REACT_CONSTANT_PROVIDER`
- `REACT_GET_CONSTANTS`
- `REACT_CONSTANT`
- `REACT_EVENT`
- `REACT_FUNCTION`

### REACT_INIT

`REACT_INIT` attaches to a function.
Such function is called before a turbo module is used in JavaScript,
there is no deterministic timing about when this function will be called,
and also no guarantee about in which thread this function will be called.

```c++
REACT_INIT(Initialize)
void Initialize(winrt::Microsoft::ReactNative::ReactContext const& reactContext) noexcept;
```

### REACT_METHOD

### REACT_SYNC_METHOD

### REACT_CONSTANT_PROVIDER

### REACT_GET_CONSTANTS

### REACT_CONSTANT

### REACT_EVENT

### REACT_FUNCTION