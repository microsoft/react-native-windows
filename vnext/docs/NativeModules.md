# Native Modules and React Native Windows

*Both this documentation and the underlying code is a work in progress. You can see the current state of working code here: [packages/microsoft-reactnative-sampleapps](../../packages/microsoft-reactnative-sampleapps)*

Sometimes an app needs access to a platform API that React Native doesn't have a corresponding module for yet. Maybe you want to reuse some existing .NET code without having to reimplement it in JavaScript, or write some high performance, multi-threaded code for image processing, a database, or any number of advanced extensions.

React Native was designed such that it is possible for you to write real native code and have access to the full power of the platform. This is a more advanced feature and we don't expect it to be part of the usual development process, however it is essential that it exists. If React Native doesn't support a native feature that you need, you should be able to build it yourself.

>NOTE: If you are building a widget that has a UI component, check out the [Native UI Component guide](./ViewManagers.md).

## Overview 

Native modules contain (or wrap) native code which can then be exposed to JS. To accomplish this in React Native for Windows vNext, at a high level you must:

1. Author your native module, which is the class that calls your native code.
   1. Add custom attributes to the class. These attributes allow you to define methods, properties, constants, and events that can be referenced from JavaScript.
1. Register your native module. Note that native modules defined within your app are automatically registered.
   1. Add the package to your React Native application.
1. Use your native module from your JavaScript code.

React Native for Windows supports authoring native modules in both C# and C++. Examples of both are provided below. 

> NOTE: If you are unable to use the reflection-based annotation approach, you can define native modules directly using the ABI. This is outlined in the [Writing Native Modules without using Attributes](./NativeModulesAdvanced.md) document. 

## Sample Native Module (C#)

### Attributes
| Attribute | Use |
|--|--|
| `ReactModule` | Specifies the class is a native module. |
| `ReactMethod` | Specifies an asynchronous method. |
| `ReactSyncMethod` | Specifies a synchronous method. |
| `ReactConstant` | Specifies a field or property that represents a constant. |
| `ReactConstantProvider` | Specifies a method that provides a set of constants. |
| `ReactEvent` | Specifies a field or property that represents an event. |


### 1. Authoring your Native Module

Here is a sample native module written in C# called `FancyMath`. It is a simple class that defines two numerical constants and a method 'add'.

*FancyMath.cs*
```csharp
using System;
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
  [ReactModule]
  class FancyMath
  {
    [ReactConstant]
    public double E = Math.E;

    [ReactConstant("Pi")]
    public double PI = Math.PI;

    [ReactMethod("add")]
    public double Add(double a, double b)
    {
        return a + b;
    }
  }
}
```

First off, you see that we're making use of the `Microsoft.ReactNative.Managed` shared library, which provides the easiest (and recommended) experience for authoring native modules. `Microsoft.ReactNative.Managed` provides the mechanism that discovers the native module annotations to build bindings at runtime.

The `ReactModule` attribute says that the class is a ReactNative native module. It has an optional parameter for the name visible to JavaScript and the name of the event emitter. By default both these names are the same as the class name. You can overwrite the name that JavaScript references or overwrite the event emitter name like this: `[ReactModule("math", EventEmitterName = "mathEmitter")]`.

The `[ReactConstant]` attribute is how you can define constants. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override the name like this: `[ReactConstant("Pi")]`.

The `[ReactMethod]` attribute is how you define methods. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. As before, you can optionally customize the name like this: `[ReactMethod("add")]`.


### 2. Registering your Native Module

> IMPORTANT NOTE: When you create a new project via the CLI, the generated `ReactNativeHost` class will automatically register all native modules defined within the app. **You will not need to manually register native modules that are defined within your app's scope, as they will be registered automatically.**

Now, we want to register our new `FancyMath` module with React Native so we can use it from JavaScript code. To do this, first we're going to create a `ReactPackageProvider` which implements [Microsoft.ReactNative.IReactPackageProvider](../Microsoft.ReactNative/IReactPackageProvider.idl).

*ReactPackageProvider.cs*
```csharp
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
  public sealed class ReactPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      packageBuilder.AddAttributedModules();
    }
  }
}
```

Here we've implemented the `CreatePackage` method, which receives `packageBuilder` to build contents of the package. Since we use reflection to discover and bind native module, we call `AddAttributedModules` extension method to register all native modules in our assembly that have the `ReactModule` attribute.

Now that we have the `ReactPackageProvider`, it's time to register it within our React Native host application's native code. To do that, we're going to look for the class which inherits from `Microsoft.ReactNative.ReactNativeHost`, the default of which (if you created your app using the React Native for Windows vNext CLI) is called `MainReactNativeHost`.

*MainReactNativeHost.cs*
```csharp
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace NativeModuleSample
{
  sealed class MainReactNativeHost : ReactNativeHost
  {
    /* ... Other Code ... */

    protected override IReadOnlyList<IReactPackageProvider> PackageProviders
    {
      get
      {
        return new IReactPackageProvider[] { new FancyMathPackageProvider() };
      }
    }
  }
}
```

### 3. Using your Native Module in JS

Now we have a Native Module which is registered with React Native Windows. How do we access it in JS? Here's a simple RN app:

*NativeModuleSample.js*
```js
import React, { Component } from 'react';
import {
  AppRegistry,
  Alert,
  NativeModules,
  Text,
  View,
} from 'react-native';

class NativeModuleSample extends Component {
  _onPressHandler() {
    NativeModules.FancyMath.add(
      /* arg a */ NativeModules.FancyMath.Pi,
      /* arg b */ NativeModules.FancyMath.E,
      /* callback */ function (result) {
        Alert.alert(
          'FancyMath',
          `FancyMath says ${NativeModules.FancyMath.Pi} + ${NativeModules.FancyMath.E} = ${result}`,
          [{ text: 'OK' }],
          {cancelable: false});
      });
  }

  render() {
    return (
      <View>
         <Text>FancyMath says PI = {NativeModules.FancyMath.Pi}</Text>
         <Text>FancyMath says E = {NativeModules.FancyMath.E}</Text>
         <Button onPress={this._onPressHandler} title="Click me!"/>
      </View>);
  }
}

AppRegistry.registerComponent('NativeModuleSample', () => NativeModuleSample);
```

To access your native modules, you need to import `NativeModules` from `react-native`. All of the native modules registered with your host application (including both the built-in ones that come with React Native for Windows vNext in addition to the ones you've added) are available as members of `NativeModules`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous nature of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addition to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.

## Sample Native Module (C++)

> NOTE: C++ does not have custom attributes and reflection as C#. Instead we use macros to simulate use of custom attributes and C++ templates to implement the binding. The binding is done during compilation time and there is virtually no overhead at runtime.

### Attributes
| Attribute | Use |
|--|--|
| `REACT_MODULE` | Specifies the class is a native module. |
| `REACT_METHOD` | Specifies an asynchronous method. |
| `REACT_SYNCMETHOD` | Specifies a synchronous method. |
| `REACT_CONSTANT` | Specifies a field or property that represents a constant. |
| `REACT_CONSTANTPROVIDER` | Specifies a method that provides a set of constants. |
| `REACT_EVENT` | Specifies a field or property that represents an event. |

### 1. Authoring your Native Module

Here is a sample native module written in C++ called `FancyMath`. It is a simple class that defines two numerical constants and a method 'add'.

*FancyMath.h*
```cpp
#include "NativeModules.h";

namespace NativeModuleSample
{
  REACT_MODULE(FancyMath);
  class FancyMath
  {
    REACT_CONSTANT(E);
    public double E = Math.E;

    REACT_CONSTANT(PI, "Pi");
    public double PI = Math.PI;

    REACT_METHOD(Add, "add");
    public double Add(double a, double b)
    {
        return a + b;
    }
  }
}
```

The `REACT_MODULE` macro-attribute says that the class is a ReactNative native module. It receives the class name as a first parameter. All other macro-attributes also receive their target as a first parameter. The `REACT_MODULE` has an optional parameter for the name visible to JavaScript and the name of the event emitter. By default both these names are the same as the class name. They can be provided explicitly as the following: `REACT_MODULE(FancyMath, "math", "mathEmitter")`.

Then we define constants, and it's as easy as creating a public field and giving it a `REACT_CONSTANT` macro-attribute. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override this by specifying an argument in the `REACT_CONSTANT` attribute (hence `Pi` instead of `PI`).

It's just as easy to add custom methods, by attributing a public method with `REACT_METHOD`. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. Again, we've specified the optional `name` argument in the `REACT_METHOD` macro-attribute so in JS we call `add` instead of `Add`.

#### 2. Registering your Native Module

> IMPORTANT NOTE: **NYI** When you create a new project via the CLI, the generated `ReactNativeHost` class will automatically register all native modules defined within the app. **You will not need to manually register native modules that are defined within your app's scope, as they will be registered automatically.**

Now, we want to register our new `FancyMath` module with React Native so we can use it from JavaScript code. To do this, first we're going to create a `ReactPackageProvider` which implements [Microsoft.ReactNative.IReactPackageProvider](../Microsoft.ReactNative/IReactPackageProvider.idl).
It starts with defining an .idl file:

*ReactPackageProvider.idl*
```csharp
namespace NativeModuleSample {

runtimeclass ReactPackageProvider : Microsoft.ReactNative.IReactPackageProvider
{
  ReactPackageProvider();
};
```

After that we add the .h and.cpp files:

*ReactPackageProvider.h*
```cpp
#pragma once
#include "ReactPackageProvider.g.h"

namespace winrt::NativeModuleSample::implementation {

struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider>
{
  ReactPackageProvider() = default;
  void CreatePackage(Microsoft::ReactNative::IReactPackageBuilder const& packageBuilder);
};

} // namespace winrt::NativeModuleSample::implementation

namespace winrt::NativeModuleSample::factory_implementation {

struct ReactPackageProvider : ReactPackageProviderT<
                                     ReactPackageProvider,
                                     implementation::ReactPackageProvider>
{
};

} // namespace winrt::NativeModuleSample::factory_implementation
```

*ReactPackageProvider.cpp*
```cpp
#include "pch.h"
#include "ReactPackageProvider.h"
#include "ReactPackageProvider.g.cpp"
#include "FancyMath.h"

using namespace winrt::Microsoft::ReactNative;
using namespace Microsoft::ReactNative;

namespace winrt::NativeModuleSample::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const& packageBuilder)
{
  AddAttributedModules(packageBuilder);
}

} // namespace winrt::NativeModuleSample::implementation

```

Here we've implemented the `CreatePackage` method, which receives `packageBuilder` to build contents of the package. Since we use macros and templates to discover and bind native module, we call `AddAttributedModules` function to register all native modules in our DLL that have the `REACT_MODULE` macro-attribute.

Now that we have the `ReactPackageProvider`, it's time to register it within our React Native host application's native code. To do that, we're going to look for the class which inherits from `Microsoft.ReactNative.ReactNativeHost`, the default of which (if you created your app using the React Native for Windows vNext CLI) is called `MainReactNativeHost`.

*MainReactNativeHost.h*
```cpp
namespace NativeModuleSample
{
  struct MainReactNativeHost : MainReactNativeHostT<MainReactNativeHost>
  {
    /* ... Other Code ... */

    IVectorView<IReactPackageProvider> PackageProviders()
    {
      auto packages = single_threaded_vector<IReactPackageProvider>({
        make<ReactPackageProvider>()
      });
    return packages.GetView();
  };
}
```

### 3. Using your Native Module in JS

Now we have a Native Module which is registered with React Native Windows. How do we access it in JS? Here's a simple RN app:

*NativeModuleSample.js*
```js
import React, { Component } from 'react';
import {
  AppRegistry,
  Alert,
  NativeModules,
  Text,
  View,
} from 'react-native';

class NativeModuleSample extends Component {
  _onPressHandler() {
    NativeModules.FancyMath.add(
      /* arg a */ NativeModules.FancyMath.Pi,
      /* arg b */ NativeModules.FancyMath.E,
      /* callback */ function (result) {
        Alert.alert(
          'FancyMath',
          `FancyMath says ${NativeModules.FancyMath.Pi} + ${NativeModules.FancyMath.E} = ${result}`,
          [{ text: 'OK' }],
          {cancelable: false});
      });
  }

  render() {
    return (
      <View>
         <Text>FancyMath says PI = {NativeModules.FancyMath.Pi}</Text>
         <Text>FancyMath says E = {NativeModules.FancyMath.E}</Text>
         <Button onPress={this._onPressHandler} title="Click me!"/>
      </View>);
  }
}

AppRegistry.registerComponent('NativeModuleSample', () => NativeModuleSample);
```

As you can see, to access your native modules, you need to import `NativeModules` from `react-native`. All of the native modules registered with your host application (including both the built-in ones that come with React Native for Windows vNext in addition to the ones you've added) are available as members of `NativeModules`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous nature of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addition to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.