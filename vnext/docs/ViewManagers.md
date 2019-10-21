# Native UI Components

>**This documentation and the underlying platform code is a work in progress. You can see the current state of working code here: [packages/microsoft-reactnative-sampleapps](../../packages/microsoft-reactnative-sampleapps)**

There are tons of native UI widgets out there ready to be used in the latest apps - some of them are part of the platform, others are available as third-party libraries, and still more might be in use in your very own portfolio. React Native has several of the most critical platform components already wrapped, like ScrollView and TextInput, but not all of them, and certainly not ones you might have written yourself for a previous app. Fortunately, we can wrap up these existing components for seamless integration with your React Native application.

Like the [native module guide](./NativeModules.md), this too is a more advanced guide that assumes you are somewhat familiar with UWP programming. This guide will show you how to build a native UI component, walking you through the implementation of a subset of the existing ImageView component available in the core React Native library.

Similarly to authoring native modules, at a high level you must:

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
