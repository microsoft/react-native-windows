
# Breaking changes for 0.63

## Managed C# projects
In 0.63 we have deprecated the Microsoft.ReactNative.SharedManaged project in favor of a proper library reference. If you only have cpp projects, no action is needed. If you have a C# app or C# native module, to update your project please perform the following actions:

### At least one C# project

1. Update your `.sln` file in the following ways:
   1. Replace the shared project with the regular project:
      ```diff
      - Project("{D954291E-2A0B-460D-934E-DC6B0785DB48}") = "Microsoft.ReactNative.SharedManaged", "..\..\..\vnext\Microsoft.ReactNative.SharedManaged\Microsoft.ReactNative.SharedManaged.shproj", "{67A1076F-7790-4203-86EA-4402CCB5E782}"
      + Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "Microsoft.ReactNative.Managed", "..\node_modules\react-native-windows\Microsoft.ReactNative.Managed\Microsoft.ReactNative.Managed.csproj", "{F2824844-CE15-4242-9420-308923CD76C3}"
      ```
   1. Remove the shared project section of the old project:
      ```diff
        GlobalSection(SharedMSBuildProjectFiles) = preSolution
      -   ..\..\..\vnext\Microsoft.ReactNative.SharedManaged\Microsoft.ReactNative.    SharedManaged.projitems*{67a1076f-7790-4203-86ea-4402ccb5e782}*SharedItemsImports     = 13
      ```
   1. Insert the configuration section for the new project
      ```diff
        GlobalSection(ProjectConfigurationPlatforms) = postSolution
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|ARM.ActiveCfg = Debug|ARM
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|ARM.Build.0 = Debug|ARM
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|ARM64.ActiveCfg = Debug|ARM64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|ARM64.Build.0 = Debug|ARM64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|x64.ActiveCfg = Debug|x64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|x64.Build.0 = Debug|x64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|x86.ActiveCfg = Debug|x86
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Debug|x86.Build.0 = Debug|x86
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|ARM.ActiveCfg = Release|ARM
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|ARM.Build.0 = Release|ARM
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|ARM64.ActiveCfg = Release|ARM64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|ARM64.Build.0 = Release|ARM64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|x64.ActiveCfg = Release|x64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|x64.Build.0 = Release|x64
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|x86.ActiveCfg = Release|x86
      +   {F2824844-CE15-4242-9420-308923CD76C3}.Release|x86.Build.0 = Release|x86
        EndGlobalSection
      ```
   1. Update the folder structure in the solution
       ```diff
         GlobalSection(NestedProjects) = preSolution
       -   {FCA38F3C-7C73-4C47-BE4E-32F77FA8538D} = {5EA20F54-880A-49F3-99FA-4B3FE54E8AB1}
       +   {F2824844-CE15-4242-9420-308923CD76C3} = {5EA20F54-880A-49F3-99FA-4B3FE54E8AB1}
         EndGlobalSection
       ```
1. Open file `App.xaml.cs`
   and ensure that you add a call to register the types in this through Reflection and remove the comment that the main provider handles modules in this project
   ```diff
   - PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider()); // Includes any modules in this project
   + PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider());
   + PackageProviders.Add(new ReflectionReactPackageProvider<App>());
   ```
   
   > note: this will change when codegen becomes default
1. Update your App `.csproj` file in the following way (Only if you created the project before 0.62)
   1. Check if has an import for `Microsoft.ReactNative.SharedManaged`
   1. Add the following to the top of your `.csproj`
   ```diff
      <?xml version="1.0" encoding="utf-8"?>
      <Project ToolsVersion="16.0" DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
   +    <PropertyGroup>
   +       <ReactNativeWindowsDir Condition="'$(ReactNativeWindowsDir)' == ''">$([MSBuild]::GetDirectoryNameOfFileAbove($(MSBuildThisFileDirectory), 'node_modules\react-native-windows\package.json'))\node_modules\react-native-windows\</ReactNativeWindowsDir>
   +    </PropertyGroup>
   +    <Import Project="$(ReactNativeWindowsDir)\PropertySheets\External\Microsoft.ReactNative.Uwp.CSharpApp.props" Condition="Exists('$(ReactNativeWindowsDir)\PropertySheets\External\Microsoft.ReactNative.Uwp.CSharpApp.props')" />
   ```
   1. Add the following to the bottom of your `.csproj` file:
   ```diff
   + <Import Project="$(ReactNativeWindowsDir)\PropertySheets\External\Microsoft.ReactNative.Uwp.CSharpApp.targets" Condition="Exists('$(ReactNativeWindowsDir)\PropertySheets\External\Microsoft.ReactNative.Uwp.CSharpApp.targets')" />
     </Project>
   ```
   1. Remove references to `Microsoft.ReactNative` and Xaml:
   ```diff
   - <ProjectReference Include="$(ReactNativeWindowsDir)\Microsoft.ReactNative\Microsoft.ReactNative.vcxproj">
   -   <Project>{f7d32bd0-2749-483e-9a0d-1635ef7e3136}</Project>
   -   <Name>Microsoft.ReactNative</Name>
   - </ProjectReference>
   - <PackageReference Include="Microsoft.UI.Xaml">
   -   <Version>2.3.191129002</Version>
   - </PackageReference>
   ```
   1. Remove references to `Microsoft.ReactNative.SharedManaged`
   ```diff
   - <Import Project="$(ReactNativeWindowsDir)\Microsoft.ReactNative.SharedManaged\Microsoft.ReactNative.SharedManaged.projitems" Label="Shared" />
   ```
   1. Remove imports for bundeling
   ```diff
   - <Import Project="$(ReactNativeWindowsDir)\PropertySheets\Bundle.props" />
   - <Import Project="$(ReactNativeWindowsDir)\PropertySheets\Bundle.targets" />
   ```

