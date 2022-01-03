# React Native TurboModule C++ Code Generation

## Running Codegen

`@react-native-windows/codegen` only supports reading turbo modules in Flow today.

`react-native-windows-codegen --file <FILE> --files <FILES> --namespace <NAMESPACE> --libraryName <LIBRARY>`

- `file`: An input file.
- `files`: a glob pattern for multiple input files.
- `namespace`: The namespace for the generated spec file. For example: `Microsoft::ReactNativeSpecs`.
- `libraryName`: part of the output folder for components.

## TypeScript TurboModule Syntax

(working)

## Implementing a TurboModule in C++

Checkout the following example:

- Flow definition: [NativeDialogManagerWindows.js](https://github.com/microsoft/react-native-windows/blob/main/vnext/src/Libraries/Alert/NativeDialogManagerWindows.js)
- Generated spec file: [NativeDialogManagerWindowsSpec.g.h](https://github.com/microsoft/react-native-windows/blob/main/vnext/codegen/NativeDialogManagerWindowsSpec.g.h)
- Implementation: [AlertModule.h](https://github.com/microsoft/react-native-windows/blob/main/vnext/Microsoft.ReactNative/Modules/AlertModule.h)

A TurboModule implemetation starts with:

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

**Tips**: You could use `React::` if you think `winrt::Microsoft::ReactNative::` is too long after `#include <NativeModules.h>`.

### REACT_MODULE

The syntax of `REACT_MODULE` is `REACT_MODULE(moduleStruct, [optional]moduleName, [optional]eventEmitterName)`.

- `moduleStruct`: the struct name that implements the TurboModule without namespace.
- `moduleName`: the module name visible to JavaScript. The default value is `L"moduleStruct"`.
- `eventEmitterName`: the default event emitter name. The default value is `L""`, which is `RCTDeviceEventEmitter`.

### Members

Any member in the struct implementing TurboModule functions should be attached with an attribute like `REACT_MODULE`.
Here is the list of all available attributes:

- `REACT_INIT`
- `REACT_SYNC_METHOD`
- `REACT_METHOD`
- `REACT_GET_CONSTANTS`
- `REACT_CONSTANT_PROVIDER`
- `REACT_CONSTANT`
- `REACT_EVENT`
- `REACT_FUNCTION`

### REACT_INIT

`REACT_INIT` attaches to a function.
Such function is called before a TurboModule is used in JavaScript,
there is no deterministic timing about when this function will be called,
and also no guarantee about in which thread this function will be called.

The syntax of `REACT_INIT` is `REACT_INIT(method)`.

- `method`: the C++ method to call to initialize this TurboModule.
```c++
REACT_INIT(Initialize)
void Initialize(winrt::Microsoft::ReactNative::ReactContext const& reactContext) noexcept;
```

### REACT_SYNC_METHOD

`REACT_SYNC_METHOD` attaches to a function.
Such function is exposed in the TurboModule to JavaScript.
There is no guarantee about in which thread this function will be called.

The syntax of `REACT_SYNC_METHOD` is `REACT_SYNC_METHOD(method, [optional]methodName)`

- `method`: the C++ method to call.
- `methodName`: the method name that is visible to JavaScript. The default value is `L"method"`.

```c++
REACT_SYNC_METHOD(AddSync)
int AddSync(int x, int y) noexcept
{
    return x + y;
}

REACT_SYNC_METHOD(AddSyncStatic)
static int AddSync(int x, int y) noexcept
{
    return x + y;
}
```

### REACT_METHOD

`REACT_METHOD` is an async version of `REACT_SYNC_METHOD`.
The attached function must returns `void`
and its last argument must be `winrt::Microsoft::ReactNative::ReactPromise<T> const& result`.

Call `result.Resolve` to finish the function call with a result.

Call `result.Reject` to finish the function call with an error.

```c++
REACT_METHOD(AddAsync)
void AddAsync(int x, int y, winrt::Microsoft::ReactNative::ReactPromise<int> const& result) noexcept
{
    result.Resolve(x + y);
}

REACT_METHOD(AddAsyncStatic)
static void AddAsync(int x, int y, winrt::Microsoft::ReactNative::ReactPromise<int> const& result) noexcept
{
    result.Reject("error!");
    // accepting char const*, wchar_t const* and winrt::Microsoft::ReactNative::ReactError
}
```

### REACT_GET_CONSTANTS

`REACT_GET_CONSTANTS` implements the sync method `getConstants` in a TurboModule.
`getConstants` returns an object literal without any argument.

The syntax of `REACT_GET_CONSTANTS` is `REACT_GET_CONSTANTS(method)`.

- `method`: the C++ method to call. Whatever the name is, it becomes `getConstants` in JavaScript.

Although they are semantically the same in JavaScript but `REACT_GET_CONSTANTS(method)` could never be replaced by `REACT_SYNC_METHOD(method, getConstants)`.

```c++
REACT_GET_CONSTANTS(GetConstants)
MyStruct GetConstants() noexcept {
    return { ... };
}

REACT_GET_CONSTANTS(GetConstantsStatic)
static MyStruct GetConstants() noexcept {
    return { ... };
}
```

**Important**: If multiple `REACT_GET_CONSTANTS`, `REACT_CONSTANT_PROVIDER` and `REACT_CONSTANT` appear in a module struct,
calling `getConstants` in JavaScript gets the result of all these methods combined together.

**Important**: If `getConstants` are in the generated spec class,
then a single `REACT_GET_CONSTANTS` becomes the only way to implement this member.

### REACT_CONSTANT_PROVIDER

`REACT_CONSTANT_PROVIDER` is a weak-typed version of `REACT_GET_CONSTANTS`.
The attached function must returns `void`
and its only argument must be `winrt::Microsoft::ReactNative::ReactConstantProvider& provider`.

The syntax of `REACT_CONSTANT_PROVIDER` is `REACT_CONSTANT_PROVIDER(method)`.

- `method`: the C++ method to call. Whatever the name is, it becomes `getConstants` in JavaScript.

```c++
REACT_CONSTANT_PROVIDER(GetConstants)
void GetConstants(winrt::Microsoft::ReactNative::ReactConstantProvider& provider) noexcept
{
    provider.Add(L"name", value);
    ...
}

REACT_CONSTANT_PROVIDER(GetConstantsStatic)
static void GetConstants(winrt::Microsoft::ReactNative::ReactConstantProvider& provider) noexcept
{
    provider.Add(L"name", value);
    ...
}
```

**Important**: If multiple `REACT_GET_CONSTANTS`, `REACT_CONSTANT_PROVIDER` and `REACT_CONSTANT` appear in a module struct,
calling `getConstants` in JavaScript gets the result of all these methods combined together.

### REACT_CONSTANT

`REACT_CONSTANT`

`REACT_CONSTANT` adds a field to `getConstants`.

The syntax of `REACT_CONSTANT` is `REACT_CONSTANT(field)`.

- `field`: the C++ field to read.

```c++
REACT_CONSTANT(Constant1)
const std::string Constant1 {"Something"};

REACT_CONSTANT(Constant2)
static constexpr int Constant2 = 100;
```

**Important**: If multiple `REACT_GET_CONSTANTS`, `REACT_CONSTANT_PROVIDER` and `REACT_CONSTANT` appear in a module struct,
calling `getConstants` in JavaScript gets the result of all these methods combined together.

### REACT_EVENT

`REACT_EVENT` implements an event.
`REACT_EVENT` attaches to a `std::function` field.

The syntax of `REACT_EVENT` is `REACT_EVENT(field, [optional]eventName, [optional]eventEmitterName)`.

- `field`: the C++ field to store a JavaScript callback.
- `eventName`: the event name visible to JavaScript. The default value is `L"field"`.
- `eventEmitterName`: the JavaScript module event emitter name. If this argument is not set, the `eventEmitterName` in `REACT_MODULE` will be used.

```c++
REACT_EVENT(AnEvent)
std::function<void(int, std::string const&)> AnEvent;
```

### REACT_FUNCTION

`REACT_FUNCTION` defines a JavaScript implemented function.
after a TurboModule is loaded,
the attached field will be initialized with a value,
calling this field executes some code registered in JavaScript.

The syntax of `REACT_FUNCTION` is `REACT_FUNCTION(field, [optional]functionName, [optional]moduleName)`.

- `field`: the C++ field to store a JavaScript function.
- `functionName`: the JavaScript function name. The default value is `L"field"`.
- `moduleName`: the JavaScript module event emitter name. If this argument is not set, the `moduleName` in `REACT_MODULE` will be used.

```c++
REACT_FUNCTION(AddValues, addValues, RandomModuleName)
std::function<void(int a, int b)> AddValues;
```

```JavaScript
import { NativeModules } from 'react-native';

class RandomModuleName {
    addValues(a, b) {
        // do something
    }
}

global.__fbBatchedBridge.registerLazyCallableModule('RandomModuleName', () => new RandomModuleName());
```

### Type Projections

Here are a list of supported types in TurboModule.

| TypeScript | Flow | C++ |
| ---------- | ---- | --- |
| `string` | `*` | `std::string` |
| `number` | `*` | `double` |
| `float (number)` | `*` | `double` |
| `double (number)` | `*` | `double` |
| `Int32 (number)` | `*` | `int` |
| `boolean` | `*` | `bool` |
| `T[]` | `*` | `std::vector<T>` |
| `readonly T[]` | `$ReadOnlyArray<T>` | `std::vector<T>` |
| `Array` | `*` | `winrt::Microsoft::ReactNative::JSValueArray` |
| `object` | `*` | `winrt::Microsoft::ReactNative::JSValue` |
| `{ literal-type }` | `{\| literal-type \|}` | A generated C++ struct |
| `RootTag` | `*` | double |
| `T` | `ReactNull` | `?T` | `std::optional<T>` |
| `function (parameters) => void` | `(arguments) => void` | `std::function<void(parameters)>` |

The following types are only valid for function return types

| TypeScript | Flow | C++ |
| ---------- | ---- | --- |
| `void` | `*` | `void` |
| `Promise<T>` | `*` | `winrt::Microsoft::ReactNative::ReactPromise<T>` (becomes the last argument) |

**Important**:

- `float`, `double`, `Int32` and `ReactNull` must be a type definition imported from another file.
- `*` means the syntax is the same between TypeScript and Flow.
- Try your best not to use `Array` and `object` in the JavaScript TurboModule definition.