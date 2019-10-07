# Native Modules and React Native Windows

*This documentation is very much a work in progress, however you can see the current state of working code in the [packages/microsoft-reactnative-sampleapps](../../packages/microsoft-reactnative-sampleapps)*

React Native can interop with your own custom native code from within your JavaScript app code. There are two main scenarios for developers: *Native Modules* and *View Managers*.

Native Modules are for exposing general purpose native code to JS.

View Managers are for exposing new native XAML controls as React Native Components to use within JSX.

## Native Modules

Native modules contain (or wrap) native code which can then be exposed to JS. To accomplish this in RNW, at a high level you must:

1. Author a class that calls your native code. There are no requirements that the class is inherited from a specific class or interface.
2. Add custom attributes to your class:
   1. `ReactModule` for the class.
   2. `ReactMethod` or `ReactSyncMethod` for asynchronous or synchronous methods.
   3. `ReactConstant` for fields or properties that represent constants.
   4. `ReactConstantProvider` for methods that provide a set of constants.
   5. `ReactEvent` for fields or properties that represent events.
3. Register your new native module in a ReactNative package. A package is a public class that implements `IReactPackageProvider` interface.
4. Add the package to your React Native application.
5. In JavaScript code:
   1. Read native module constants.
   2. Subscribe to the native module events.
   3. Invoke the native module methods. 

As an alternative to using classes with attributes, we can add native module definitions directly in the React package.

### Sample Native Module using Microsoft.ReactNative.Managed (recommended)

#### 1. Authoring your Native Module

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

First off, you see that we're making use of the `Microsoft.ReactNative.Managed` shared library, which provides the easiest (and recommended) experience for authoring native modules. Rather than providing native module meta information directly, we use reflection to discover it, and then use LINQ Expressions to build bindings at runtime.

The `ReactModule` attribute says that the class is a ReactNative native module. It has an optional parameter for the name visible to JavaScript and the name of the event emitter. By default both these names are the same as the class name. They can be provided explicitly as the following: `[ReactModule("math", EventEmitterName = "mathEmitter")]`.

Then we define constants, and it's as easy as creating a public property or field and giving it a `[ReactConstant]` attribute. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override this by specifying an argument in the `[ReactConstant]` attribute (hence `Pi` instead of `PI`).

It's just as easy to add custom methods, by attributing a public method with `[ReactMethod]`. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. Again, we've specified the optional `Name` argument in the `[ReactMethod]` attribute so in JS we call `add` instead of `Add`.

#### 2. Registering your Native Module

Now, we want to register our new `FancyMath` module with React Native so we can use it from JavaScript code. To do this, first we're going to create a `FancyMathPackageProvider` which implements [Microsoft.ReactNative.IReactPackageProvider](../Microsoft.ReactNative/IReactPackageProvider.idl).

*FancyMathPackageProvider.cs*
```csharp
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
  public sealed class FancyMathPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      packageBuilder.AddAttributedModules();
    }
  }
}
```

Here we've implemented the `CreatePackage` method, which receives `packageBuilder` to build contents of the package. Since we use reflection to discover and bind native module, we call `AddAttributedModules` extension method to register all native modules in our assembly that have the `ReactModule` attribute.

Now that we have the `FancyMathPackageProvider`, it's time to register it within our React Native host application's native code. To do that, we're going to look for the class which inherits from `Microsoft.ReactNative.ReactNativeHost`, the default of which (if you created your app using the RNW CLI) is called `MainReactNativeHost`.

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

#### 3. Using your Native Module in JS

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

As you can see, to access your native modules, you need to import `NativeModules` from `react-native`. All of the native modules registered with your host application (including both the built-in ones that come with RNW in addition to the ones you've added) are available as members of `NativeModules`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous nature of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addition to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.

### Sample Native Module using just Microsoft.ReactNative (not recommended)

Now for reference, it is possible to write directly against `INativeModule` however it is not recommended unless you know what you're doing.

#### 1. Authoring your Native Module

Here is the same `FancyMath` native module we created above, but we do not use custom attributes and going to implement registration of its members ourselves without using reflection.

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

#### 2. Registering your Native Module

Here's our `FancyMathPackageProvider` where we do not use Reflection to register native module members.

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

As you can see, it is possible to use the API directly, but the code looks a little bit more complicated. The most complexity is related to the value serialization and deserialization. We can choose using helper methods for that and then the code would look a little bit simpler:

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

#### 3. Using your Native Module in JS

Consumption of your Native Module in JS is the same as in the example above.

## View Managers

View Managers are a form of specialized native modules which expose native UI controls as React Native Components. In RNW, this means you have access to the world of rich XAML controls. Similarly to authoring native modules, at a high level you must:

1. Author a Windows Runtime Class which implements [Microsoft.ReactNative.IViewManager](../Microsoft.ReactNative/IViewManager.idl) and understands how to interact with the native UI.
2. Register your new `IViewManager` within the native code of your React Native host application.
3. Reference the new Component within your React Native JSX code.

*Open Questions:*

* Do we need to expose (custom) shadow nodes?
* Do we need to create JS wrappers so the Component can be referenced (at all, or more naturally) in JSX?
* Do we ever want to support non-FrameworkElement views?
* How do XAML controls fire events into RN?

### Sample View Manager using Microsoft.ReactNative.Managed (recommended)

#### 1. Authoring your View Manager 

Here is a sample view manager written in C# called `CircleViewManager`.

*FancyMath.cs*
```csharp
using System;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Shapes;

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

namespace ViewManagerSample
{
    public sealed class CircleViewManager : IManagedViewManager
    {
        public string Name => "Circle";

        public FrameworkElement CreateView() => new Ellipse();

        [ViewManagerPropertySetter(Name = "radius")]
        public static void SetRadius(Ellipse circle, double? value)
        {
            if (value.HasValue)
            {
                circle.Width = value.Value * 2.0;
                circle.Height = value.Value * 2.0;
            }
            else
            {
                circle.ClearValue(FrameworkElement.WidthProperty);
                circle.ClearValue(FrameworkElement.HeightProperty);
            }
        }
    }
}
```

As with native modules, we're making use of the `Microsoft.ReactNative.Managed` library, which provides the easiest (and recommended) experience for authoring view managers. Rather than implement the `IViewManager` interface directly with our class, we're implementing [IManagedViewManager](../Microsoft.ReactNative.Managed/ManagedViewManager.cs), which will be passsed as a parameter to the `ManagedViewManager` helper class.

To fulfill `IManagedViewManager`, we only have to implement two things: the `Name` property getter and the `CreateView()` method.

The `Name` string specifies the name of the React Native Component that you will use in JSX, and that this view manager know how to work with. In this case, we're creating a new Component called "Circle".

The `CreateView()` method returns the XAML control (anything derived from `FrameworkElement`) that will be added to the native UI tree when React Native asks for a "Circle". In this case, we return a new `Windows.UI.Xaml.Shapes.Ellipse`.

Now we need to define some properties that are unique to our Circle component. We don't need to specify every possible property (most properties common to all `FrameworkElement`s will already be taken care of) so we just need to focus on the ones that are unique to Circle.

In this case, we want a `radius` property, so we need to create a public method attributed with a `[ViewManagerPropertySetter]` attribute. We specify `Name` argument set to `radius` which let's React Native know, when an app wants to set the `radius` property on a Circle component, this is the mehtod to call.

All property setter methods should take two parameters - an instance of the native XAML control, and the value of the property being set. This value will come from React Native. As you can see, our `SetRadius()` method follows a simple pattern: it takes nullable double - if there is a value, we set the width and height of the Ellipse to 2x that value. If that value is null, we clear the width and height of the Ellipse.

#### 2. Registering your View Manager

As with native modules, we want to register our new `CircleViewManager` module with React Native so we can actually use it. To do this, first we're going to create a `CircleViewManagerPackage` which implements `Microsoft.ReactNative.IReactPackage`.

*CircleViewManagerPackage.cs*
```csharp
using System;
using System.Collections.Generic;

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

namespace ViewManagerSample
{
    public sealed class CircleViewManagerPackage : IReactPackage
    {
        /*
            Other Code
        */

        public IReadOnlyList<IViewManager> CreateViewManagers(ReactContext reactContext)
        {
            return new List<IViewManager>() {
                new ManagedViewManager(new CircleViewManager()),
            };
        }
    }
}
```

Here we've implemented the `CreateViewManagers` method, which returns a read-only collection of `IViewManager` instances. We're only going have oneview manager in this package, and as previously mentioned, we're going to use the `ManagedViewManager` adapter class provided by `Microsoft.ReactNative.Managed`.

`ManagedViewManager` implements `IViewManager` by reflecting over an `IManagedViewManager` which uses the `[ViewManagerPropertySetter]` attributes. So in our package, we simply pass an instance of `CircleViewManager` into `ManagedViewManager`.

Now that we have a `CircleViewManagerPackage`, it's time to actually register it within our React Native host application's native code. To do that, we're going to look for the class which inherits from `Microsoft.ReactNative.ReactNativeHost`, the default of which (if you created your app using the RNW CLI) is called `MainReactNativeHost`.

*MainReactNativeHost.cs*
```csharp
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace ViewManagerSample
{
    sealed class MainReactNativeHost : ReactNativeHost
    {
        /*
            Other Code
        */

        protected override IReadOnlyList<IReactPackage> Packages
        {
            get
            {
                return new IReactPackage[] { new CircleViewManagerPackage() };
            }
        }
    }
}
```

#### 3. Using your View Manager in JSX

*TODO:* Example should be able to use `<Circle>` component syntax, but need to figure out how to import the component type.

### Sample View Manager using just Microsoft.ReactNative (not recommended)

Now for reference, it is possible and supported to write directly against `IViewManager` however it is not recommended unless you know what you're doing.

#### 1. Authoring your View Manager

Here is the same `CircleViewManager` we created above, but here we write it directly against `IViewManager` without the benefit of `Microsoft.ReactNative.Managed`.

*CircleViewManager.cs*
```csharp
using System;
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace ViewManagerSample
{
    public sealed class CircleViewManagerABI : IViewManager
    {
        public string Name => "CircleABI";

        public FrameworkElement CreateView() => new Ellipse();

        public IReadOnlyDictionary<string, string> NativeProps => _nativeProps ?? (_nativeProps = new Dictionary<string, string>(1)
        {
            { "radius", "number" },
        });
        private IReadOnlyDictionary<string, string> _nativeProps;

        public void UpdateProperties(FrameworkElement view, IReadOnlyDictionary<string, object> propertyMap)
        {
            if (view is Ellipse circle)
            {
                foreach (var property in propertyMap)
                {
                    switch (property.Key)
                    {
                        case "radius":
                            if (property.Value is double propertyValue)
                            {
                                circle.Width = propertyValue * 2.0;
                                circle.Height = propertyValue * 2.0;
                            }
                            else if (null == property.Value)
                            {
                                circle.ClearValue(FrameworkElement.WidthProperty);
                                circle.ClearValue(FrameworkElement.HeightProperty);
                            }
                            break;
                    }
                }
            }
        }
    }
}
```

As you can see, it's a little more complicated, and more "glue" code that you're responsible for.

#### 2. Registering your View Manager

Here's our `CircleViewManager` again, and again this time we're not depending on the `Microsoft.ReactNative.Managed` library.

*FancyMathPackage.cs*
```csharp
using System;
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace ViewManagerSample
{
    public sealed class CircleViewManagerPackage : IReactPackage
    {
        /*
            Other Code
        */

        public IReadOnlyList<IViewManager> CreateViewManagers(ReactContext reactContext)
        {
            return new List<IViewManager>() {
                new CircleViewManager(),
            };
        }
    }
}
```

And as for the rest, once we have the `IReactPackage`, registering that package is the same as in the example above.

#### 3. Using your View Manager in JSX

Consumption of your new View in JS is the same as in the example above.
