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
- [IntegrationTestScripts\IntegrationTests.njsproj](#IntegrationTests-Node-Project)
- [IntegrationTests\React.Windows.IntegrationTests.vcxproj](#React.Windows.IntegrationTests)
- [JSI\Desktop\JSI.Desktop.vcxproj](#JSI.Desktop)
- [JSI\Shared\JSI.Shared.vcxitems](#JSI.Shared)
- [JSI\Universal\JSI.Universal.vcxproj](#JSI.Universal)
- [JSI.Desktop.UnitTests\JSI.Desktop.UnitTests](#JSI.Desktop.UnitTests)
- **[Microsoft.ReactNative\Microsoft.ReactNative.vcxproj](#Microsoft.ReactNative)**
- [Microsoft.ReactNative.Cxx\Microsoft.ReactNative.Cxx.vcxitems](#Microsoft.ReactNative.Cxx)
- [Microsoft.ReactNative.Managed\Microsoft.ReactNative.Managed.csproj](#Microsoft.ReactNative.Managed)
- [Microsoft.ReactNative.SharedManaged\Microsoft.ReactNative.SharedManaged.projitems](#Microsoft.ReactNative.SharedManaged)
- [ReactCommon\ReactCommon.vcxproj](#ReactCommon)
- [Shared\Shared.vcxitems](#Shared)

## Common Projects

### Common
*Static Library*<br/>
Functionality shared between React and JSI projects that do not depend on React or JSI functionality.

### Shared
*Shared Items (no build artifact)*<br/>
Holds sources common to both Windows variants, that require different build configuration
(i.e. compiler flags, language standard).

### ReactCommon
*Static Library*<br/>
React Native core, cross-platform C++ types and interfaces.<br/>
Sources provided as part of the `react-native` Node dependency. Not part of this repository.<br/>
See https://github.com/facebook/react-native/tree/v0.60.0/ReactCommon.

### Folly
*Static Library*<br/>
Folly variant type system for JavaScript/C++ interoperability.<br/>
Sources provided as part of the `react-native` Node dependency. Not part of this repository.

### Chakra
*Shared Items (no build artifact)*<br/>
ChakraCore bridging layer. May use different compiler flags between Windows variants.

### JSI.Shared
*Shared Items (no build artifact)*<br/>
Code shared between [JSI\Desktop\JSI.Desktop.vcxproj](#JSI.Desktop) and [JSI\Universal\JSI.Universal.vcxproj](#JSI.Universal).

### Microsoft.ReactNative.Cxx
*Shared Items (no build artifact)*<br/>
Contains helpers to simplify authoring C++/WinRT native modules on top of [Microsoft.ReactNative](#Microsoft.ReactNative).

### Microsoft.ReactNative.Managed
*Shared Items (no build artifact)*<br/>
Contains helpers to simplify authoring C# native modules on top of [Microsoft.ReactNative](#Microsoft.ReactNative).

### Microsoft.ReactNative.SharedManaged
*Shared Items (no build artifact)*<br/>
This shared project is to be deprecated in some time in favor of the library [Microsoft.ReactNative.Managed](#Microsoft.ReactNative.Managed).
For now it contaisn the same code as the library.

### React.Windows.IntegrationTests
*Static Library*<br/>
Common framework for running out of process and/or full React instance testing.

### IntegrationTests (Node Project)
*MSBuild Node project. For reading/editing purposes only (no build artifact)*<br/>
Set of JavaScript component tests for [RNTester](https://github.com/facebook/react-native/tree/v0.60.0/RNTester).
Sources provided as part of the `react-native` Node dependency. Not part of this repository.<br/>
See https://github.com/facebook/react-native/tree/v0.60.0/IntegrationTests.

## Windows Desktop Projects

### React.Windows.Desktop
*Static Library*<br/>
Set of Native Modules, View Managers and Executors for Windows Desktop.

### React.Windows.Desktop.DLL
*Dynamic Library*<br/>
Shared library that exports the intended public API surface for [React.Windows.Desktop](#React.Windows.Desktop).<br/>
**Main artifact to use in Windows Desktop applications.**

### React.Windows.Desktop.UnitTests
*VSTest Dynamic Library*<br/>
Set of isolated (mocked) tests for types defined in [React.Windows.Desktop](#React.Windows.Desktop).

### React.Windows.Desktop.IntegrationTests
*VSTest Dynamic Library*<br/>
Set of component tests that validate functionality against external runtime components
(i.e. networking servers, file system, React Native applications, external processes).<br/>
Validates [React.Windows.Desktop.DLL](#React.Windows.Desktop.DLL).

### JSI.Desktop
*Static Library*<br/>
ChakraCore based JSI::Runtime implementation.

### JSI.Desktop.UnitTests
*Google Test Executable*<br/>
Set of unit tests for jsi::runtime.

### FollyWin32
*Static Library*<br/>
Superset of Folly APIs only available and required by [React.Windows.Desktop](#React.Windows.Desktop).<br/>
Sources provided as part of the `react-native` Node dependency. Not part of this repository.

## Windows Universal Projects

### Microsoft.ReactNative
*Windows Runtime Component*</br>
The primary Windows Universal entry point and public API surface for React Native Windows.<br/>
**Main artifact to use in Windows Universal applications.**

### JSI.Universal
*Static Library*<br/>
Chakra based JSI::Runtime implementation.
