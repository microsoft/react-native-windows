# Native Modules and React Native Windows

*This documentation is very much a work in progress*

React Native can interop with your own custom native code from within your JavaScript app code. There are two main scenarios for developers: *Native Modules* and *View Managers*.

Native Modules are for exposing general purpose native code to JS.

View Managers are for exposing new native Xaml controls as React Native Components to use within JS.

## Native Modules

Native Modules contain (or wrap) native code which can then be exposed to JS. To accomplish this in RNW, at a high level you must:

1. Author a Windows Runtime Class which implements `Microsoft.ReactNative.INativeModule` and calls your native code.
2. Register your new `INativeModule` within the native code of your React Native host application.
3. Invoke your NativeModule from within your Raact Native JS code.

### Sample Native Module using Microsoft.ReactNative.Managed (recommended)

#### 1. Authoring your Native Module

Here is a sample native module written in C# called `FancyMath`. It is a simple class which provides its name, an initialization method, two numerical constants and a method.

*FancyMath.cs*
```
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

The `Name` string specifies the name of the object that you will use in JS to access the Native Module's constants and methods. In this case, we've called it "FancyMath".

The `Initialize()` method specifies any native code that you need React Native to run in itialiaze your Native Module. For this sample, FancyMath doesn't have any initialization.

Now we'll define some constants, and it's as easy as creating a public property or field and giving it an `[NativeModuleConstant]` attribute. Here FancyMath has defined two constants: `E` and `Pi`. By default, the name exposed to JS will be the same name as the field (`E` for `E`), but you can override this by specifying a `Name` argument in the `[NativeModuleConstant]` attribute (hence `Pi` instead of `PI`).

It's just as easy to add custom methods, by attributing a method with `[NativeModuleMethod]`. In FancyMath we have one method, `add`, which takes two doubles and returns their sum. Again, we've specified the optional `Name` argument in the `[NativeModuleMethod]` attribute so in JS we call `add` instead of `Add`.

#### 2. Registering your Native Module

Now, we want to register our new `FancyMath` module with React Native so we can actually use it. To do this, first we're going to create a `FancyMathPackage` which implements `Microsoft.ReactNative.IReactPackage`.

*FancyMathPackage.cs*
```
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
    }
}
```

Here we've implemented the `CreateNativeModules` method, which returns a read-only collection of `INativeModule` instances. We're only going have one Native Module in this package, and as previously mentioned, we're using `ManagedNativeModule` effectively as a wrapper around a `FancyMath` instance.

Now that we have a `FancyMathPackage`, it's time to actually register it within our React Native host application's native code. To do that, we're going to look for the class which inherits from `Microsoft.ReactNative.ReactNativeHost`, the default of which (if you created your app using the RNW CLI) is called `MainReactNativeHost`.

*MainReactNativeHost.cs*
```
using System.Collections.Generic;

using Microsoft.ReactNative;

namespace NativeModuleSample
{
    sealed class MainReactNativeHost : ReactNativeHost
    {
        /* Other Overrides */

        protected override IReadOnlyList<IReactPackage> Packages
        {
            get
            {
                return new[] { new FancyMathPackage() };
            }
        }
    }
}
```

### 3. Using your Native Module in JS

Now we have a Native Module which is registered with React Native Windows. How do we access it in JS? Here's a simple RN app:

*NativeModuleSample.js*
```
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

As you can see, to access your Native Modules, you need to import `NativeModules` from `react-native`. All of NativeModules registered with your host application (including both the built-in ones that come with RNW in addition to the ones you've added) are available as members of `NativeModules`.

To access our `FancyMath` constants, we can simply call `NativeModules.FancyMath.E` and `NativeModules.FancyMath.Pi`.

Calls to methods are a little different due to the asynchronous natire of the JS engine. If the native method returns nothing, we can simply call the method. However, in this case `FancyMath.add()` returns a value, so in addtion to the two necessary parameters we also include a callback function which will be called with the result of `FancyMath.add()`. In the example above, we can see that the callback raises an Alert dialog with the result value.

### Sample Native Module using just Microsoft.ReactNative (not recommended)

While writing directly against `INativeModule` is supported, it is not recommended unless you know what you're doing.

#### 1. Authoring your Native Module

Here is the same `FancyMath` native module we created above, but here we write it directly against `INativeModule` without the benefit of `Microsoft.ReactNative.Managed`.

*FancyMath.cs*
```
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

#### 2. Registering your Native Module

Here's our `FancyMathPackage` again, and again this time we're not depending on the `Microsoft.ReactNative.Managed` library.

*FancyMathPackage.cs*
```
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
    }
}
```

And as for the rest, once we have the `IReactPackage`, registering that package in *MainReactNativeHost.cs* is the same as above.

### 3. Using your Native Module in JS

Consumption of your Native Module in JS is the same as in *NativeModuleSample.js* above.

## View Managers