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

> NOTE: If you are unable to use the reflection-based annotation approach, you can define native modules directly using the ABI. This is outlined in the [Native Modules and React Native Windows (Advanced Topics)](./NativeModulesAdvanced.md) document. 

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
        double result = a + b;
        AddEvent(result);
        return result;
    }

    [ReactEvent]
    public ReactEvent<double> AddEvent { get; set; }
  }
}
```

First off, you see that we're making use of the `Microsoft.ReactNative.Managed` shared library, which provides the easiest (and recommended) experience for authoring native modules. `Microsoft.ReactNative.Managed` provides the mechanism that discovers the native module annotations to build bindings at runtime.

The `[ReactModule]` attribute says that the class is a ReactNative native module. It has an optional parameter for the module name visible to JavaScript and optionally the name of a registered event emitter. By default, the name visible to JavaScript is the same as the class name, and the default event emitter is `RCTDeviceEventEmitter`.

You can overwrite the JavaScript module name like this: `[ReactModule("math")]`.

You can specify a different event emitter like this: `[ReactModule(EventEmitter = "mathEmitter")]`.

> NOTE: Using the default event emitter, `RCTDeviceEventEmitter`, all native event names must be **globally unique across all native modules** (even the ones built-in to RN). However, specifying your own event emitter means you'll need to create and register that too. This process is outlined in the [Native Modules and React Native Windows (Advanced Topics)](./NativeModulesAdvanced.md) document.

The `[ReactConstant]` attribute is how you can define constants. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override the name like this: `[ReactConstant("Pi")]`.

The `[ReactMethod]` attribute is how you define methods. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. As before, you can optionally customize the name like this: `[ReactMethod("add")]`.

The `[ReactEvent]` attribute is how you define events. In FancyMath we have one event, `AddEvent`, which uses the `ReactEvent<double>` delegate, where the double represents the type of the event data. Now whenever we invoke the `AddEvent` delegate in our native code (as we do above), an event named `"AddEvent"` will be raised in JavaScript. As before, you could have optionally customized the name in JS like this: `[ReactEvent("addEvent")]`. 

### 2. Registering your Native Module

> IMPORTANT NOTE: When you create a new project via the CLI, the generated `ReactApplication` class will automatically register all native modules defined within the app. **You will not need to manually register native modules that are defined within your app's scope, as they will be registered automatically.**

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

Now that we have the `ReactPackageProvider`, it's time to register it within our `ReactApplication`. We do that by simply adding the proviver to the `PackageProviders` property.

*App.xaml.cs*
```csharp
using Microsoft.ReactNative;

namespace SampleApp
{
    sealed partial class App : ReactApplication
    {
        public App()
        {
            /* Other Init Code */

            PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider()); // Includes any modules in this project
            PackageProviders.Add(new NativeModuleSample.ReactPackageProvider());

            /* Other Init Code */
        }
    }
}
```

This example assumes that the `NativeModuleSample.ReactPackageProvider` we created above is in a different project (assembly) than our application. However you'll notice that by default we also added a `Microsoft.ReactNative.Managed.ReactPackageProvider`.

The `Microsoft.ReactNative.Managed.ReactPackageProvider` is a convenience that makes sure that all native modules and view managers defined within the app project automatically get registered. So if you're creating your native modules directly within the app project, you won't actually want to define a separate `ReactPackageProvider`. 

### 3. Using your Native Module in JS

Now we have a Native Module which is registered with React Native Windows. How do we access it in JS? Here's a simple RN app:

*NativeModuleSample.js*
```js
import React, { Component } from 'react';
import {
  AppRegistry,
  Alert,
  Text,
  View,
} from 'react-native';

import { NativeModules, NativeEventEmitter } from 'react-native';

const FancyMathEventEmitter = new NativeEventEmitter(NativeModules.FancyMath);

class NativeModuleSample extends Component {

  componentDidMount() {
    // Subscribing to FancyMath.AddEvent
    FancyMathEventEmitter.addListener('AddEvent', eventHandler, this);
  }

  componentWillUnmount() {
    // Unsubscribing from FancyMath.AddEvent
    FancyMathEventEmitter.removeListener('AddEvent', eventHandler, this);
  }

  eventHandler(result) {
    console.log("Event was fired with: " + result);
  }

  _onPressHandler() {
    // Calling FancyMath.add method
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

To access your native modules, you need to import `NativeModules` from `react-native`. All of the native modules registered with your host application (including both the built-in ones that come with React Native for Windows vNext in addition to the ones you've added) are available as members of `NativeModules`. Since our native modules fires events, we're also bringing in `NativeEventEmitter`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous nature of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addition to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.

For events, you'll see that we created an instance of `NativeEventEmitter` passing in our `NativeModules.FancyMath` module, and called it `FancyMathEventEmitter`. We can then use the `FancyMathEventEmitter.addListener()` and `FancyMathEventEmitter.removeListener()` methods to subscribe to our `FancyMath.AddEvent`. In this case, when `AddEvent` is fired in the native code, `eventHandler` will get called, which logs the result to the console log.

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
      double result = a + b;
      AddEvent(result);
      return result;
    }

    REACT_EVENT(AddEvent);
    std::function<void(double)> AddEvent;
  }
}
```

The `REACT_MODULE` macro-attribute says that the class is a ReactNative native module. It receives the class name as a first parameter. All other macro-attributes also receive their target as a first parameter. `REACT_MODULE` has an optional parameter for the module name visible to JavaScript and optionally the name of a registered event emitter. By default, the name visible to JavaScript is the same as the class name, and the default event emitter is `RCTDeviceEventEmitter`.

You can overwrite the JavaScript module name like this: `REACT_MODULE(FancyMath, "math")`.

You can specify a different event emitter like this: `REACT_MODULE(FancyMath, "math", "mathEmitter")`.

> NOTE: Using the default event emitter, `RCTDeviceEventEmitter`, all native event names must be **globally unique across all native modules** (even the ones built-in to RN). However, specifying your own event emitter means you'll need to create and register that too. This process is outlined in the [Native Modules and React Native Windows (Advanced Topics)](./NativeModulesAdvanced.md) document.

Then we define constants, and it's as easy as creating a public field and giving it a `REACT_CONSTANT` macro-attribute. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override this by specifying an argument in the `REACT_CONSTANT` attribute (hence `Pi` instead of `PI`).

It's just as easy to add custom methods, by attributing a public method with `REACT_METHOD`. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. Again, we've specified the optional `name` argument in the `REACT_METHOD` macro-attribute so in JS we call `add` instead of `Add`.

To add custom events, we attribute a `std::function<void(double)>` delegate with `REACT_EVENT`, where the double represents the type of the event data. Now whenever we invoke the `AddEvent` delegate in our native code (as we do above), an event named `"AddEvent"` will be raised in JavaScript. As before, you could have optionally customized the name in JS like this: `REACT_EVENT(AddEvent, "addEvent")`.

#### 2. Registering your Native Module

> IMPORTANT NOTE: When you create a new project via the CLI, the generated `ReactApplication` class will automatically register all native modules defined within the app. **You will not need to manually register native modules that are defined within your app's scope, as they will be registered automatically.**

Now, we want to register our new `FancyMath` module with React Native so we can use it from JavaScript code. To do this, first we're going to create a `ReactPackageProvider` which implements [Microsoft.ReactNative.IReactPackageProvider](../Microsoft.ReactNative/IReactPackageProvider.idl). It starts with defining an .idl file:

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

// NOTE: You must include the headers of your native modules here in
// order for the AddAttributedModules call below to find them.
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

Now that we have the `ReactPackageProvider`, it's time to register it within our `ReactApplication`. We do that by simply adding the proviver to the `PackageProviders` property.

*App.cpp*
```c++
#include "pch.h"

#include "App.h"
#include "ReactPackageProvider.h"

namespace winrt::SampleApp::implementation {

App::App() noexcept {
  /* Other Init Code */

  PackageProviders().Append(make<ReactPackageProvider>()); // Includes all modules in this project
  PackageProviders().Append(winrt::NativeModuleSample::ReactPackageProvider());

  /* Other Init Code */
}

} // namespace winrt::SampleApp::implementation
```

This example assumes that the `NativeModuleSample::ReactPackageProvider` we created above is in a different project (assembly) than our application. However you'll notice that by default we also added a `SampleApp::ReactPackageProvider`.

The `SampleApp::ReactPackageProvider` is a convenience that makes sure that all native modules and view managers defined within the app project automatically get registered. So if you're creating your native modules directly within the app project, you won't actually want to define a separate `ReactPackageProvider`. 

### 3. Using your Native Module in JS

Now we have a Native Module which is registered with React Native Windows. How do we access it in JS? Here's a simple RN app:

*NativeModuleSample.js*
```js
import React, { Component } from 'react';
import {
  AppRegistry,
  Alert,
  Text,
  View,
} from 'react-native';

import { NativeModules, NativeEventEmitter } from 'react-native';

const FancyMathEventEmitter = new NativeEventEmitter(NativeModules.FancyMath);

class NativeModuleSample extends Component {

  componentDidMount() {
    // Subscribing to FancyMath.AddEvent
    FancyMathEventEmitter.addListener('AddEvent', eventHandler, this);
  }

  componentWillUnmount() {
    // Unsubscribing from FancyMath.AddEvent
    FancyMathEventEmitter.removeListener('AddEvent', eventHandler, this);
  }

  eventHandler(result) {
    console.log("Event was fired with: " + result);
  }

  _onPressHandler() {
    // Calling FancyMath.add method
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

To access your native modules, you need to import `NativeModules` from `react-native`. All of the native modules registered with your host application (including both the built-in ones that come with React Native for Windows vNext in addition to the ones you've added) are available as members of `NativeModules`. Since our native modules fires events, we're also bringing in `NativeEventEmitter`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous nature of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addition to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.

For events, you'll see that we created an instance of `NativeEventEmitter` passing in our `NativeModules.FancyMath` module, and called it `FancyMathEventEmitter`. We can then use the `FancyMathEventEmitter.addListener()` and `FancyMathEventEmitter.removeListener()` methods to subscribe to our `FancyMath::AddEvent`. In this case, when `AddEvent` is fired in the native code, `eventHandler` will get called, which logs the result to the console log.

