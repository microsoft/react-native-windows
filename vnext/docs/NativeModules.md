# Native Modules Guide for React Native Windows (vnext)

React Native supports interop between JavaScript and native code through the use of native modules.

Custom native modules provide methods and constants implemented in native language that are exposed for use in JavaScript. For example, if you needed access to a thrid-party native image processing library, you could implement a custom native module as a wrapper to expose that library's code to your React Native app.

Custom view managers are specialized native modules that expose native UI controls for use as React Native components. For example, if you wanted to use a third-party XAML control in your React Native app you would create a custom view manager to register that control as a component that you could use then in your React Native app.

## Current Support

| UWP Host Application | Custom Native Modules | Custom View Managers |
|:---------------------|:---------------------:|:--------------------:|
| C++/WinRT            | Yes (C++/WinRT only)  | Yes (C++/WinRT only) |
| C#, C++/CX           | Yes (C#, C++/CX only) | Not yet              |


## High-Level Overview

### C++/WinRT UWP Host Application

#### Creating a Custom Native Module

1. Subclass `facebook::xplat::module::CxxModule`
  1. Override `getName()` with your module's name
  2. Override `getMethods()` with the list of methods your module will provide
  3. Override `getConstants()` with the constants that your module will provide
2. Subclass `facebook::react::NativeModuleProvider`
  1. Override `GetModules()` to return a collection that contains an instance of your subclassed `CxxModule`
3. When you call `react::uwp::CreateReactInstance()`, pass in your subclassed `NativeModuleProvider`

#### Creating a Custom View Manager

1. Subclass `react::uwp::FrameworkElementViewManager`
2. Subclass `react::uwp::ShadowNodeBase`
3. Subclass `react::uwp::ViewManagerProvider`
  1. Override `GetViewManagers()` to return a collection that contains an instance of your subclassed `FrameworkElementViewManager`
4. When you call `react::uwp::CreateReactInstance()`, pass in your subclassed `ViewManagerProvider`

## C#, C++/CX UWP Host Application

#### Creating a Custom Native Module

1. Create a class which implements `react::uwp::IModule`
2. Call `RegisterModule`, on your ReactInstance with your `IModule` (*Note:* this will be changed with issue [#2392](https://github.com/microsoft/react-native-windows/issues/2392))

#### Creating a Custom View Manager

This has not yet been implemented. See issue [#2298](https://github.com/microsoft/react-native-windows/issues/2298).

## FAQ

### How do I use a custom C++/WinRT native module in my C# (or C++/CX) UWP Host Application (or vice-versa)?

This has not yet been implemented. See issue [#2393](https://github.com/microsoft/react-native-windows/issues/2393).
