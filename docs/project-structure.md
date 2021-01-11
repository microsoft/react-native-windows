# MSBuild Project Structure

This is a description of each of the core projects, their purpose and output artifacts.<br/>
Sample applications are not covered.

## Alphabetical Index
- [Common\Common.vcxproj](#Common)
- [Chakra\Chakra.vcxitems](#Chakra)
- [Desktop\React.Windows.Desktop.vcxproj](#React.Windows.Desktop)
- **[Desktop.DLL\React.Windows.Desktop.DLL.vcxproj](#React.Windows.Desktop.DLL)**
- [Desktop.IntegrationTests\React.Windows.Desktop.IntegrationTests.vcxproj](#React.Windows.Desktop.IntegrationTests)
- [Desktop.UnitTests\React.Windows.Desktop.UnitTests.vcxproj](#React.Windows.Desktop.UnitTests)
- [Folly\Folly.vcxproj](#Folly)
- [FollyWin32\FollyWin32.vcxproj](#FollyWin32)
- [IntegrationTests\React.Windows.IntegrationTests.vcxproj](#React.Windows.IntegrationTests)
- **[Microsoft.ReactNative\Microsoft.ReactNative.vcxproj](#Microsoft.ReactNative)**
- [Microsoft.ReactNative.Cxx\Microsoft.ReactNative.Cxx.vcxitems](#Microsoft.ReactNative.Cxx)
- [Microsoft.ReactNative.Managed\Microsoft.ReactNative.Managed.csproj](#Microsoft.ReactNative.Managed)
- [Microsoft.ReactNative.SharedManaged\Microsoft.ReactNative.SharedManaged.projitems](#Microsoft.ReactNative.SharedManaged)
- [ReactCommon\ReactCommon.vcxproj](#ReactCommon)
- [ReactCommon.UnitTests\ReactCommon.UnitTests.vcxproj](#ReactCommon.UnitTests)
- [Shared\Shared.vcxitems](#Microsoft.ReactNative.Shared)

## Common Projects

### Common
*Static Library*\
Functionality shared between React and JSI projects.

### Microsoft.ReactNative.Shared
*Shared Items (no build artifact)*\
Holds sources common to both Windows variants, that require different build configuration
(i.e. compiler flags, language standard).

### ReactCommon
*Static Library*\
React Native core, cross-platform C++ types and interfaces.\
Sources provided as part of the `react-native` Node dependency. Not part of this repository.\
See https://github.com/facebook/react-native/tree/v0.62.0/ReactCommon.

### Folly
*Static Library*\
Folly variant type system for JavaScript/C++ interoperability.\
Sources provided as part of the `react-native` Node dependency. Not part of this repository.

### Chakra
*Shared Items (no build artifact)*\
ChakraCore bridging layer. May use different compiler flags between Windows variants.

### Microsoft.ReactNative.Cxx
*Shared Items (no build artifact)*\
Contains helpers to simplify authoring C++/WinRT native modules on top of [Microsoft.ReactNative](#Microsoft.ReactNative).

### Microsoft.ReactNative.Managed
*Managed Library*\
Contains helpers to simplify authoring C# native modules on top of [Microsoft.ReactNative](#Microsoft.ReactNative).

### Microsoft.ReactNative.SharedManaged
Deprecated: This is still here so that customers get a specific error message on how to upgrade their project.

### React.Windows.IntegrationTests
*Static Library*\
Common framework for running out of process and/or full React instance testing.

### IntegrationTests (Node Project)
*MSBuild Node project. For reading/editing purposes only (no build artifact)*\
Set of JavaScript component tests for [RNTester](https://github.com/facebook/react-native/tree/v0.62.0/RNTester).
Sources provided as part of the `react-native` Node dependency. Not part of this repository.\
See https://github.com/facebook/react-native/tree/v0.62.0/IntegrationTests.

## Windows Desktop Projects

### React.Windows.Desktop
*Static Library*\
Set of Native Modules, View Managers and Executors for Windows Desktop.

### React.Windows.Desktop.DLL
*Dynamic Library*\
Shared library that exports the intended public API surface for [React.Windows.Desktop](#React.Windows.Desktop).\
**Main artifact to use in Windows Desktop applications.**

### React.Windows.Desktop.UnitTests
*VSTest Dynamic Library*\
Set of isolated (mocked) tests for types defined in [React.Windows.Desktop](#React.Windows.Desktop).

### React.Windows.Desktop.IntegrationTests
*VSTest Dynamic Library*\
Set of component tests that validate functionality against external runtime components
(i.e. networking servers, file system, React Native applications, external processes).\
Validates [React.Windows.Desktop.DLL](#React.Windows.Desktop.DLL).

### ReactCommon.UnitTests
*Google Test Application*\
Gathers tests within `react-native\ReactCommon`.\
Its main purpose is to validate JSI with any given engine (V8, ChakraCore, etc).

### FollyWin32
*Static Library*\
Superset of Folly APIs only available and required by [React.Windows.Desktop](#React.Windows.Desktop).\
Sources provided as part of the `react-native` Node dependency. Not part of this repository.

## Windows Universal Projects

### Microsoft.ReactNative
*Windows Runtime Component*</br>
The primary Windows Universal entry point and public API surface for React Native Windows.\
**Main artifact to use in Windows Universal applications.**
