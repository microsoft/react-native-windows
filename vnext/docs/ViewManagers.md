# Native UI Components

>**This documentation and the underlying platform code is a work in progress. You can see the current state of working code here: [packages/microsoft-reactnative-sampleapps](../../packages/microsoft-reactnative-sampleapps)**

There are tons of native UI widgets out there ready to be used in the latest apps - some of them are part of the platform, others are available as third-party libraries, and still more might be in use in your very own portfolio. React Native has several of the most critical platform components already wrapped, like ScrollView and TextInput, but not all of them, and certainly not ones you might have written yourself for a previous app. Fortunately, we can wrap up these existing components for seamless integration with your React Native application.

Like the [native module guide](./NativeModules.md), this too is a more advanced guide that assumes you are somewhat familiar with UWP programming. This guide will show you how to build a native UI component, walking you through the implementation of a subset of the existing ImageView component available in the core React Native library.

## Overview

Similarly to authoring native modules, at a high level you must:

1. Author a ViewManager which defines a new Component type and understands how to create and interact with the native UI widget.
2. Register your new ViewManager within the native code of your React Native host application.
3. Reference the new Component within your React Native JSX code.

## Sample ViewManager (C#)

### Attributes
| Attribute | Use |
|--|--|
| `ViewManagerExportedViewConstant` | Specifies a field or property that represents a constant. |
| `ViewManagerProperty` | Specifies a method to be called to set a property on an instance of a native UI widget. |
| `ViewManagerCommand` | Specifies a method that can be called on an instance of a native UI widget. |

For this sample, assume we have the following `CustomUserControl` that we want to use in React Native.

*CustomUserControl.cs*
```csharp
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ViewManagerSample
{
    public sealed class CustomUserControl : Control
    {
        public static DependencyProperty LabelProperty { get; private set; }

        public string Label
        {
            get
            {
                return (string)GetValue(LabelProperty);
            }
            set
            {
                SetValue(LabelProperty, value);
            }
        }

        static CustomUserControl()
        {
            LabelProperty = DependencyProperty.Register(
                nameof(Label),
                typeof(string),
                typeof(CustomUserControl),
                new PropertyMetadata(default(string))
                );
        }

        public CustomUserControl()
        {
            DefaultStyleKey = typeof(CustomUserControl);
        }
    }
}
```

```xml
<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:local="using:ViewManagerSample">

    <Style TargetType="local:CustomUserControl" >
        <Setter Property="Template">
            <Setter.Value>
                <ControlTemplate TargetType="local:CustomUserControl">
                    <Border
                        Background="{TemplateBinding Background}"
                        BorderBrush="{TemplateBinding BorderBrush}"
                        BorderThickness="{TemplateBinding BorderThickness}">
                        <TextBlock Foreground="{TemplateBinding Foreground}" Text="{TemplateBinding Label}" TextAlignment="Center" />
                    </Border>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>
</ResourceDictionary>
```

#### 1. Authoring your View Manager 

Here is a sample view manager written in C# called `CustomUserControlViewManager`.

*FancyMath.cs*
```csharp
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;

using Microsoft.ReactNative.Managed;
using System.Collections.Generic;

namespace ViewManagerSample
{
    internal class CustomUserControlViewManager : AttributedViewManager<CustomUserControlCS>
    {
        [ViewManagerProperty("label")]
        public void SetLabel(CustomUserControl view, view, string value)
        {
            if (null != value)
            {
                view.SetValue(CustomUserControl.LabelProperty, value);
            }
            else
            {
                view.ClearValue(CustomUserControl.LabelProperty);
            }
        }

        [ViewManagerProperty("color")]
        public void SetColor(CustomUserControl view, Brush value)
        {
            if (null != value)
            {
                view.SetValue(Control.ForegroundProperty, value);
            }
            else
            {
                view.ClearValue(Control.ForegroundProperty);
            }
        }

        [ViewManagerProperty("backgroundColor")]
        public void SetBackgroundColor(CustomUserControl view, Brush value)
        {
            if (null != value)
            {
                view.SetValue(Control.BackgroundProperty, value);
            }
            else
            {
                view.ClearValue(Control.BackgroundProperty);
            }
        }
    }
}
```

#### 2. Registering your View Manager

As with native modules, we want to register our new `CustomUserControlViewManager` with React Native so we can actually use it. To do this, first we're going to create a `ReactPackageProvider` which implements [Microsoft.ReactNative.IReactPackageProvider](../Microsoft.ReactNative/IReactPackageProvider.idl).

*ReactPackageProvider.cs*
```csharp
using Microsoft.ReactNative.Managed;

namespace ViewManagerSample
{
  public sealed class ReactPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      packageBuilder.AddViewManagers();
    }
  }
}
```

Here we've implemented the `CreatePackage` method, which receives `packageBuilder` to build contents of the package. Since we use reflection to discover and bind native module, we call `AddViewManagers` extension method to register all the view managers in our assembly.

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
            PackageProviders.Add(new ViewManagerSample.ReactPackageProvider());

            /* Other Init Code */
        }
    }
}
```

This example assumes that the `ViewManagerSample.ReactPackageProvider` we created above is in a different project (assembly) than our application. However you'll notice that by default we also added a `Microsoft.ReactNative.Managed.ReactPackageProvider`.

The `Microsoft.ReactNative.Managed.ReactPackageProvider` is a convenience that makes sure that all native modules and view managers defined within the app project automatically get registered. So if you're creating your view managers directly within the app project, you won't actually want to define a separate `ReactPackageProvider`. 

#### 3. Using your View Manager in JSX

*ViewManagerSample.js*
```js
import React, { Component } from 'react';
import {
  AppRegistry,
  requireNativeComponent,
  StyleSheet,
  View,
} from 'react-native';

let CustomUserControl = requireNativeComponent('CustomUserControl');

class ViewManagerSample extends Component {
  render() {
    return (
      <View style={styles.container}>
         <CustomUserControl style={styles.customcontrol} label="CustomUserControlCS!" ref={(ref) => { this._cuccsRef = ref; }} />
      </View>);
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  customcontrol: {
    color: '#333333',
    backgroundColor: '#006666',
    width: 200,
    height: 20,
    margin: 10,
  },
});

AppRegistry.registerComponent('ViewManagerSample', () => ViewManagerSample);
```

## Sample ViewManager (C++)

*TODO*
