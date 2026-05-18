# React Native TurboModule C++ Code Generation

## Running Codegen

`@react-native-windows/codegen` only supports reading turbo modules in Flow today.

[react-native-tscodegen](https://github.com/microsoft/react-native-tscodegen/tree/master/packages/RN-TSCodegen) has a TypeScript front-end that works with `react-native-windows-codegen`.
The `RN-TSCodegen-Types` package is experimental,
which provides useful TypeScript to assist the codegen like `Int32` and `ReactNull`.
But there is no such constraint that these types must be imported from `RN-TSCodegen-Types`,
if any technical issue prevent you from using `RN-TSCodegen-Types`,
you could copy them in your own file and import them to your turbo module
(these types cannot be defined in the same file where the turbo module is defined).

`react-native-windows-codegen --file <FILE> --files <FILES> --namespace <NAMESPACE> --libraryName <LIBRARY>`

- `file`: An input file.
- `files`: a glob pattern for multiple input files.
- `namespace`: The namespace for the generated spec file. For example: `Microsoft::ReactNativeSpecs`.
- `libraryName`: part of the output folder for components.

After successfully executed the codegen,
a C++ spec class will be generated in a header file.

## TypeScript TurboModule Syntax

A TypeScript TurboModule definition file must contain:

- Exactly one module spec interface inheriting from `TurboModule`.
- An export statement in this format `export default TurboModuleRegistry.getEnforcing<SPEC-INTERFACE-TYPE>('MODULE-NAME');`

```TypeScript
import {Int32, TurboModule, TurboModuleRegistry} from `react-native`;

type TwoNumbers = {
    a: Int32;
    b: Int32;
};

export interface Spec extends TurboModule {
    getConstants(): {
        const1: boolean;
        const2: number;
        const3: string;
    };

    addSync(a: Int32, b: Int32): Int32;
    addAsync(twoNumbers: TwoNumbers): Promise<Int32>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('SampleTurboModule');
```

## Implementing a TurboModule in C++

Checkout the following example:

- Flow definition: [NativeDialogManagerWindows.js](https://github.com/microsoft/react-native-windows/blob/main/vnext/src-win/Libraries/NativeModules/specs/NativeDialogManagerWindows.js)
- Generated spec file after compilatation in the enlistment: `vnext\codegen\NativeDialogManagerWindowsSpec.g.h`
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

In order to make a TurboModule member accessible in JavaScript,
one of the following attributes must be applied to the C++ member:

- `REACT_INIT`
- `REACT_SYNC_METHOD`
- `REACT_METHOD`
- `REACT_GET_CONSTANTS`
- `REACT_CONSTANT_PROVIDER`
- `REACT_CONSTANT`
- `REACT_EVENT`
- `REACT_FUNCTION`

### REACT_INIT

`REACT_INIT` applies to a function.
Such function is called after members with `REACT_EVENT` and `REACT_FUNCTION` are initialized.
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

`REACT_SYNC_METHOD` applies to a function.
Such function is exposed in the TurboModule to JavaScript.
Such function will be called synchronously from the JS thread.

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

There are four ways to define an async function:
- Define `winrt::Microsoft::ReactNative::ReactPromise<T> const& result` as the last argument.
- Define one callback as the last argument for returning a value, such function cannot raise errors.
- Define two callbacks as the last two arguments for returning a value or raising an error.
- Define a function which returns the result normally, instead of using callbacks or `ReactPromise`.

All `REACT_METHOD` functions are exposed in JavaScript as a function returning `Promise`,
it doesn't matter in which of the four ways a function is defined.

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
The applied function must return `void`
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
`REACT_EVENT` applies to a `std::function` field.

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
After a TurboModule is loaded,
the applied field will be initialized with a value,
calling this field executes some code registered in JavaScript.

The syntax of `REACT_FUNCTION` is `REACT_FUNCTION(field, [optional]functionName, [optional]moduleName)`.

- `field`: the C++ field to store a JavaScript function.
- `functionName`: the JavaScript function name. The default value is `L"field"`.
- `moduleName`: the JavaScript callable module name. If this argument is not set, the `moduleName` in `REACT_MODULE` will be used.

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
| `string` | `*` | `std::string` or `std::wstring` according to options |
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

## Reflecting REACT_MODULE applied structs

**Please ignore this section when using `REACT_MODULE` with `react-native-windows`,
all interfaces have already been properly implemented.**

Calling `MakeModuleProvider<T>` or `MakeTurboModuleProvider<T>` returns a `ReactModuleProvider` from a `REACT_MODULE` applied struct `T`.
`ReactModuleProvider` is a delegate, by passing a `IReactModuleBuilder` object as an argument, all reflectable members in the `REACT_MODULE` applied struct will be enumerated and passed to `IReactModuleBuilder`.
Your job is to implement `IReactModuleBuilder` to collect all members of a struct.

### IReactModuleBuilder

Here are all members in `IReactModuleBuilder`:

- `void AddInitializer(InitializerDelegate)`.
`InitializerDelegate` is a delegate taking `IReactContext` as an argument.
A `REACT_MODULE` applied struct expects all `InitializerDelegate` to be called before calling any other members.
It is `IReactModuleBuilder`'s responsibility to prepare the `IReactContext` object.

- `AddConstantProvider(ConstantProviderDelegate)`.
`ConstantProviderDelegate` is a delegate taking `IJSValueWriter` as an argument.
A `REACT_MODULE` applied struct could provide multiple `ConstantProviderDelegate`,
each delegate writes multiple members to the `IJSValueWriter`.
In order to properly accept these members,
delegates must be called between `IJSValueWriter::WriteObjectBegin` and `IJSValueWriter::WriteObjectEnd`,
after that `IJSValueWriter` stores an object with all constant values as its fields.

- `AddMethod(String, MethodReturnType, MethodDelegate)`.
`MethodDelegate` represents an asynchronized method. The return value and the exception is returned by delegates in the 3rd and 4th arguments.
`MethodReturnType` represents how this function looks like in JavaScript.
  - `MethodReturnType::Callback`. The last argument is a callback to accept the return value, other n-1 arguments are passed to the delegate in `IJSValueReader`.
  - `MethodReturnType::TwoCallbacks`. The last two arguments are callbacks for the return value and the exception, other n-2 arguments are passed to the delegate in `IJSValueReader`.
  - `MethodReturnType::Void`. All arguments are passed to the delegate in `IJSValueReader`. This delegate do not offer return value or exception.
  - `MethodReturnType::Promise`. All arguments are passed to the delegate in `IJSValueReader`.

- `AddSyncMethod(String, SyncMethodDelegate)`.
`SyncMethodDelegate` represents a synchronized method,
taking `IJSValueReader` and `IJSValueWriter` as its arguments.
The delegate will read all arguments from `IJSValueReader` as an array,
and write the return value to `IJSValueWriter`.

### IJSValueReader and IJSValueWriter

Serialization and deserialization between C++ objects and `IJSValueReader/Writer` happens automatically.
You should implement `IJSValueReader` and `IJSValueWriter` to pass arguments and read return values.

An `IJSValueReader` or `IJSValueWriter` could access objects in JSON plain text, JSI objects or whatever is available in the other side of the platform.

If you only need an abstract representation of JSON values,
`Microsoft.ReactNative.Cxx` providers such abstraction in `JSValue`, `JSValueObject` and `JSValueArray`,
with `JSValueTreeReader` and `JSValueTreeWriter` to implement `IJSValueReader` and `IJSValueWriter`.

You could add serialization supports for more types by making new overloading functions for `winrt::Microsoft::ReactNative::ReadValue` and `winrt::Microsoft::ReactNative::WriteValue`.
Another choice is to implement serialization and deserialization between these types and `JSValue`,
and `Microsoft.ReactNative.Cxx` will do the rest for you.

### IReactContext

`IReactContext` allow `REACT_MODULE` applied structs to access whatever is provided by the React Native application.