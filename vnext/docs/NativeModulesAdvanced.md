# Native Modules and React Native Windows (Advanced Topics)

>**This documentation and the underlying platform code is a work in progress. You can see the current state of working code here: [packages/microsoft-reactnative-sampleapps](../../packages/microsoft-reactnative-sampleapps)**

## Writing Native Modules without using Attributes (C#)

The [Native Modules](./NativeModules.md) page describes how you can author Native Modules in both C# and C++ using an attribute-based approach. The attribute-based approach makes it easy for you to author your native modules because it uses reflection to help React Native understand your native module. 

In rare cases, you may need to write a native module against the ABI directly without using reflection. The high-level steps remain the same:
1. Author your native module
2. Register your native module
3. Use your native module

However, your app will be responsible for handling the code that the attributes would have otherwise provided. This guide will walk through those steps.

### 1. Authoring your Native Module

Here is the same `FancyMath` native module, but we do not use custom attributes and we're instead going to implement registration of its members ourselves without using reflection.

*FancyMath.cs*
```csharp
using System;
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
  class FancyMath
  {
    public double E = Math.E;

    public double PI = Math.PI;

    public double Add(double a, double b)
    {
        return a + b;
    }
  }
}
```

### 2. Registering your Native Module

Here's our `FancyMathPackageProvider` where we manually register our native module's members.

*FancyMathPackageProvider.cs*
```csharp
namespace NativeModuleSample
{
  public sealed class FancyMathPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      packageBuilder.AddModule("FancyMath", (IReactModuleBuilder moduleBuilder) => {
        var module = new FancyMath();
        moduleBuilder.SetName("FancyMath");
        moduleBuilder.AddConstantProvider((IJSValueWriter writer) => {
          writer.WritePropertyName("E");
          writer.WriteDouble(module.E);
          writer.WritePropertyName("Pi");
          writer.WriteDouble(module.PI);
        });
        moduleBuilder.AddMethod("add", MethodReturnType.Callback,
          (IJSValueReader inputReader,
          IJSValueWriter outputWriter,
          MethodResultCallback resolve,
          MethodResultCallback reject) => {
           inputReader.ReadNext();
           inputReader.ReadNext();
           inputReader.TryGetDouble(out double a);
           inputReader.ReadNext();
           inputReader.TryGetDouble(out double b);
           double result = module.Add(a, b);
           writer.WriteArrayBegin();
           writer.WriteDouble(result);
           writer.WriteArrayEnd();
           resolve(writer);
          });
        return module;
      });
    }
  }
}
```

As you can see, it is possible to use the API directly, but the code looks a little bit more complicated. You are responsible for creating your own constant provider, serializing each constant into the `IJSValueWriter`. For methods, you are also responsible for deserializing args, calling your code, and then serializing any return values. There are some (de)serialization helper methods to make that a little bit simpler:

*FancyMathPackageProvider.cs*
```csharp
namespace NativeModuleSample
{
  public sealed class FancyMathPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      packageBuilder.AddModule("FancyMath", (IReactModuleBuilder moduleBuilder) => {
        var module = new FancyMath();
        moduleBuilder.SetName("FancyMath");
        moduleBuilder.AddConstantProvider((IJSValueWriter writer) => {
          writer.WriteProperty("E", module.E);
          writer.WriteProperty("Pi", module.PI);
        });
        moduleBuilder.AddMethod("add", MethodReturnType.Callback,
          (IJSValueReader inputReader,
          IJSValueWriter outputWriter,
          MethodResultCallback resolve,
          MethodResultCallback reject) => {
           object[] args = inputReader.ReadArgs();
           double result = module.Add((double)args[0], (double)args[1]);
           writer.WriteArgs(result);
           resolve(writer);
          });
        return module;
      });
    }
  }
}
```

It is possible to simplify the code even more by hiding the use of the value reader and writer interfaces:

*FancyMathPackageProvider.cs*
```csharp
namespace NativeModuleSample
{
  public sealed class FancyMathPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      packageBuilder.AddModule("FancyMath", (IReactModuleBuilder moduleBuilder) => {
        var module = new FancyMath();
        moduleBuilder.SetName("FancyMath");
        moduleBuilder.AddConstantProvider(() => new Dictionary<string, object> {
          ["E"] = module.E,
          ["Pi"] = module.PI
        });
        moduleBuilder.AddMethod("add", MethodReturnType.Callback, (MethodCallContext callContext) => {
          double result = module.Add((double)callContext.Args[0], (double)callContext.Args[1]);
          callContext.resolve(result);
        });
        return module;
      });
    }
  }
}
```

This code looks much better, but we are getting the overhead of boxing values that involves memory allocation for each call. The code generation that we do using LINQ Expression avoids this extra overhead. Though, the initial use of reflection and code generation has some penalty too.
From the maintenance point of view, the attributed code is much simple to support because we do not need to describe the same things in two different places.

And as for the rest of the code, once we have the `IReactPackageProvider`, registering that package is the same as in the example above that uses custom attributes.

### 3. Using your Native Module in JS

Using your native module in JS is the exact same as if the native module was defined using attributes. 

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

## Native Modules with Custom Event Emitters

By default, native modules share a common `RCTDeviceEventEmitter` which emits the actual events into JavaScript. However, that comes with the limitation that all of the native modules must have globally unique event names. If you wanted to ensure that your event names didn't collide with other events from other native modules, you could create your own `EventEmitter`.

So say we have our FancyMath module, where we've specified `"MathEmitter"` as the name of the `EventEmitter`:

*FancyMath.cs*
```csharp
using System;
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
  [ReactModule(EventEmitterName = "MathEmitter")]
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

Now, when the native code calls `AddEvent`, that will be essentially translated into a JS call of `MathEmitter.emit("AddEvent", result)`.

So in order for this to work, you will need to create and register a `MathEmitter` module. You can create the module by and you can use the existing `EventEmitter` class.

*MathEmitter.js*
```js

const EventEmitter = require('EventEmitter');
const BatchedBridge = require('BatchedBridge');

BatchedBridge.registerLazyCallableModule('MathEmitter', () => {
  return new EventEmitter();
});

```

*TODO:* We should have a better E2E example here, however in practice naitve module developers don't bother to do this, so it's impossible to find any existing examples to reference.
