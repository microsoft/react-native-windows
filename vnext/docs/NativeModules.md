# Native Modules and React Native Windows

*This documentation is very much a work in progress*

React Native can interop with your own custom native code from within your JavaScript app code. There are two main scenarios for developers: *Native Modules* and *View Managers*.

Native Modules are for exposing general purpose native code to JS.

View Managers are for exposing new native XAML controls as React Native Components to use within JSX.

## Native Modules

Native modules contain (or wrap) native code which can then be exposed to JS. To accomplish this in RNW, at a high level you must:

1. Author a Windows Runtime Class which implements `Microsoft.ReactNative.INativeModule` and calls your native code.
2. Register your new `INativeModule` within the native code of your React Native host application.
3. Invoke your NativeModule from within your Raact Native JS code.

*Open Questions:*

* How do native modules fire events into RN?

### Sample Native Module using Microsoft.ReactNative.Managed (recommended)

#### 1. Authoring your Native Module

Here is a sample native module written in C# called `FancyMath`. It is a simple class which provides its name, an initialization method, two numerical constants and a method.

*FancyMath.cs*
```csharp
using System;

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
    public sealed class FancyMath : IManagedNativeModule
    {
        public string Name => "FancyMath";

        public void Initialize() { }

        [NativeModuleConstant]
        public double E => Math.E;

        [NativeModuleConstant(Name = "Pi")]
        public double PI => Math.PI;

        [NativeModuleMethod(Name = "add")]
        public static double Add(double a, double b)
        {
            return a + b;
        }
    }
}
```

First off, you'll see that we're making use of the `Microsoft.ReactNative.Managed` library, which provides the easiest (and recommended) experience for authoring native modules. Rather than implement the `INativeModule` interface directly with our class, we're implementing `IManagedNativeModule`, which will be passsed as a parameter to the `ManagedNativeModule` helper class.

To fulfill `IManagedNativeModule`, we only have to implement two things: the `Name` property getter and the `Initialize()` method.

The `Name` string specifies the name of the object that you will use in JS to access the native module's constants and methods. In this case, we've called it "FancyMath".

The `Initialize()` method specifies any native code that you need React Native to run in initialize your native module. For this sample, FancyMath doesn't have any initialization.

Now we'll define some constants, and it's as easy as creating a public property or field and giving it an `[NativeModuleConstant]` attribute. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override this by specifying a `Name` argument in the `[NativeModuleConstant]` attribute (hence `Pi` instead of `PI`).

It's just as easy to add custom methods, by attributing a public method with `[NativeModuleMethod]`. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. Again, we've specified the optional `Name` argument in the `[NativeModuleMethod]` attribute so in JS we call `add` instead of `Add`.

#### 2. Registering your Native Module

Now, we want to register our new `FancyMath` module with React Native so we can actually use it. To do this, first we're going to create a `FancyMathPackage` which implements `Microsoft.ReactNative.IReactPackage`.

*FancyMathPackage.cs*
```csharp
using System;
using System.Collections.Generic;

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

namespace NativeModuleSample
{
    public sealed class FancyMathPackage : IReactPackage
    {
        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            return new List<INativeModule>() {
                new ManagedNativeModule(new FancyMath()),
            };
        }

        /*
            Other Code
        */
    }
}
```

Here we've implemented the `CreateNativeModules` method, which returns a read-only collection of `INativeModule` instances. We're only going have one native module in this package, and as previously mentioned, we're going to use the `ManagedNativeModule` adapter class provided by `Microsoft.ReactNative.Managed`.

`ManagedNativeModule` implements `INativeModule` by reflecting over an `IManagedNativeModule` which uses the `[NativeModuleConstant]` and `[NativeModuleMethod]` attributes. So in our package, we simply pass an instance of `FancyMath` into `ManagedNativeModule`.

Now that we have a `FancyMathPackage`, it's time to actually register it within our React Native host application's native code. To do that, we're going to look for the class which inherits from `Microsoft.ReactNative.ReactNativeHost`, the default of which (if you created your app using the RNW CLI) is called `MainReactNativeHost`.

*MainReactNativeHost.cs*
```csharp
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace NativeModuleSample
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
                return new IReactPackage[] { new FancyMathPackage() };
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
                [
                {
                    text: 'OK',
                },
                ],
                {cancelable: false},
            );
            }
        );
    }

    render() {
        return (
            <View>
            <Text>
                FancyMath says PI = {NativeModules.FancyMath.Pi}
            </Text>
            <Text>
                FancyMath says E = {NativeModules.FancyMath.E}
            </Text>
            <Button onPress={this._onPressHandler} title="Click me!"/>
            </View>
        );
    }
}
});

AppRegistry.registerComponent('NativeModuleSample', () => NativeModuleSample);
```

As you can see, to access your native modules, you need to import `NativeModules` from `react-native`. All of the native modules registered with your host application (including both the built-in ones that come with RNW in addition to the ones you've added) are available as members of `NativeModules`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous nature of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addtion to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.

### Sample Native Module using just Microsoft.ReactNative (not recommended)

Now for reference, it is possible and supported to write directly against `INativeModule` however it is not recommended unless you know what you're doing.

#### 1. Authoring your Native Module

Here is the same `FancyMath` native module we created above, but here we write it directly against `INativeModule` without the benefit of `Microsoft.ReactNative.Managed`.

*FancyMath.cs*
```csharp
using System;
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace NativeModuleSample
{
    public sealed class FancyMath : INativeModule
    {
        public string Name => "FancyMath";

        public void Initialize() { }

        public IReadOnlyDictionary<string, object> Constants => _constants ?? (_constants = new Dictionary<string, object>(1)
        {
            { "E", Math.E },
            { "Pi", Math.PI },
        });
        private IReadOnlyDictionary<string, object> _constants;

        public IReadOnlyList<MethodInfo> Methods => _methods ?? (_methods = new List<MethodInfo>(1)
        {
            new MethodInfo("add", ReturnType.Callback, (args, callback, ___) =>
            {
                double a = (double)args[0];
                double b = (double)args[1];

                double result = a + b;

                callback(new object[] { result });
            }),
        });
        private IReadOnlyList<MethodInfo> _methods;
    }
}
```

As you can see, it's a little more complicated, and more "glue" code that you're responsible for.

#### 2. Registering your Native Module

Here's our `FancyMathPackage` again, and again this time we're not depending on the `Microsoft.ReactNative.Managed` library.

*FancyMathPackage.cs*
```csharp
using System;
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace NativeModuleSample
{
    public sealed class FancyMathPackage : IReactPackage
    {
        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            return new List<INativeModule>() {
                new FancyMath()),
            };
        }

        /*
            Other Code
        */
    }
}
```

And as for the rest, once we have the `IReactPackage`, registering that package is the same as in the example above.

#### 3. Using your Native Module in JS

Consumption of your Native Module in JS is the same as in the example above.

## View Managers

View Managers are a form of specialized native modules which expose native UI controls as React Native Components. In RNW, this means you have access to the world of rich XAML controls. Similarly to authoring native modules, at a high level you must:

1. Author a Windows Runtime Class which implements `Microsoft.ReactNative.IViewManager` and understands how to interact with the native UI.
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

As with native modules, we're making use of the `Microsoft.ReactNative.Managed` library, which provides the easiest (and recommended) experience for authoring view managers. Rather than implement the `IViewManager` interface directly with our class, we're implementing `IManagedViewManager`, which will be passsed as a parameter to the `ManagedViewManager` helper class.

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
