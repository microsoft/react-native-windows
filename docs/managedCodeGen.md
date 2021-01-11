# Code generation for managed react-native modules

This feature is applicable to C# implemented modules or `ViewManager`s. Its objective is to ensure the code needed to integrate with the `react-native` application is constructed at build time instead of at runtime.

In earlier versions of react-native-windows managed modules registered themselves based on reflection at runtime. This method has a few disadvantages: 
1) the error handling was limited and only available at runtime. 
2) The cost for doing reflection was paid each and every time the app ran. 
3) .NET Native - which is used by UWP apps - has somewhat restrictions on what types/APIs can be reflected upon and complex ways of working around it.
Doing it at build time should result in developers seeing all errors up front, rather than only seeing errors for the code being tested. And the runtime cost is not incurred every single time.

## How it works
This change adds a new targets file that is imported from the [CSharpLib.targets](https://github.com/microsoft/react-native-windows/blob/master/vnext/PropertySheets/External/Microsoft.ReactNative.Uwp.CppLib.targets) and [CSharpApp.targets](https://github.com/microsoft/react-native-windows/blob/master/vnext/PropertySheets/External/Microsoft.ReactNative.Uwp.CppApp.targets). That will enable the codegen for managed projects.

Those targets will collect all the C# source files, assembly references and defines from the app (or library) and invoke the CodeGen tool to generate a C# file. This is very similar to how the translation of Xaml to csharp code behind files works.
The generated C# file will contain a class called `ReactPackageProvider` in the default namespace of your project. The generate class implements `Microsoft.ReactNative.IReactPackageProvider`. The implementation of the generated class  will:
 * Register the detected ViewManagers (classes implementing `Microsoft.ReactNative.IViewManager`)
 * Register the detected Modules (classes with attribute `[Microsoft.ReactNative.Managed.ReactModule]`)
 * Generate and register serializers for types that need to be serialized for the registred modules.
 * Generate and register custom serialization methods.

### In source mode
The targets file hooks up an MSBuild target that will ensure the tool is compiled, deployed and runnable based on the tools sources. The codegen tool is defined in the Microsoft.ReactNative.Managed.CodeGen project. It will build and deploy the tool in the current configuraiton of your project (debug or release) but will always build and run it in platform x64.

### In NuGet mode (Future)
The tool will be shipped as a binary and the steps won't have to build or deploy the tool. The shipped version in the NuGet package will only be release win-x64.
When building the apps that are checked in to our repo for testing, we have to remain running off the source version as MSBuild and NuGet are not flexible enough to build and consume a NuGet package in the same build.

## Turning on/off
Currently the feature is behind an MSBuild property `$(ReactNativeCodeGenEnabled)`.
The default at the moment is false, it is only turned  on for a single project for now which is the [SampleLibraryCS.csproj](https://github.com/microsoft/react-native-windows/blob/master/packages/microsoft-reactnative-sampleapps/windows/SampleLibraryCS/SampleLibraryCS.csproj) project, to prove it is stable. We will slowly enable it for all projects in the repo and then make it the default.

## MSBuild/NuGet Complications
MSBuild and NuGet spent a long time fighting me in mixing a NetCoreApp3.1 executable and WinRT apps in the same solution and the same build. ProjectReferences cannot be used so I had to use the `<MSBuild>` task directly in the targets and it was tricky making it build from both the customer apps as well as our main build solution and unittest (Microsoft.ReactNative.sln). In the end there are a few hacks in place to make this work.
The hookups in MSBuild do not use the latest BeforeTargets/AfterTargets feature to schedule the main work. I opted to follow the same pattern that the Xaml Codegen uses, so the React CodeGen runs after all the other codegen tools (like resgen, xamlcompile etc) to ensure people familiar with those extensions know how to work with the React Managed CodeGen.

## Future improvements
 * Add support for `Task<T>` async methods. [#5143](https://github.com/microsoft/react-native-windows/issues/5143)
 * Ship it as a NuGet package [#4546](https://github.com/microsoft/react-native-windows/issues/4546)
