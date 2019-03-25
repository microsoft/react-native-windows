# Setup Troubleshooting

### Failing build on `react-native run-windows`
#### Error:

```
Build failed with message Error: Command failed: "C:\Program Files (x86)\MSBuild\14.0\bin\msbuild.exe" 
"C:/<path_to_project>/windows/<project_name>.sln" /clp:NoSummary;NoItemAndProperty;Verbosity=minimal /nologo
/p:Configuration=debug /p:Platform=x86
. Check your build configuration.
```

#### Solution:

This error is likely caused by missing libraries on your machine. To fix this, open the solutions file, `C:\<path_to_project>\windows\<project_name>.sln`, in Visual Studio. You will be prompted to install the missing libraries. Install the preselected libraries and restart visual studio.

Here is an example of these libraries:

![Missing Libraries](img/MissingLibraries1.png)

![Missing Libraries](img/MissingLibraries2.png)

#### Error:

```
Failed to restore the NuGet packages
```

#### Solution:

This usually happens when you don't have Visual Studio installed. Install Visual Studio as listed in the [Requirements](https://github.com/Microsoft/react-native-windows#requirements), and try again.


#### Error:

```
Build failed with message Error: Must have a minimum Windows SDK version 10.0.10586.0
```

#### Solution:

This is exactly as it says, you need to install the Windows 10 SDK version >= 10.0.10586.x. Note that the latest Windows 10 SDK requires Visual Studio 2017, which React Native Windows doesn't support quite yet. Install Windows 10 SDK version 10.0.14393.x as listed in the [Requirements](https://github.com/Microsoft/react-native-windows#requirements), and try again.


#### Error:
```
ERROR: The system was unable to find the specified registry key or value.

Build failed with message Error: Command failed: "C:\Program Files\MSBuild\14.0\
bin\msbuild.exe" "C:/Users/IEUser/v41/windows/v41.sln" /clp:NoSummary;NoItemAndP
ropertyList;Verbosity=minimal /nologo /p:Configuration=Debug /p:Platform=x86 /p:
AppxBundle=Never
. Check your build configuration.
```

or

```
error APPX0002: Task 'GenerateAppxPackageRecipe' failed. 0x7F - Failed to load MRM support library.
```

#### Solution:

Re-run the standalone installer for Windows 10 SDK version 10.0.14393.x as listed in the [Requirements](https://github.com/Microsoft/react-native-windows#requirements). It appears that sometimes a partial install occurs when the SDK is installed via the Visual Studio installer. 

#### Error:
(After trying to run the project)
```
The project '___' cannot be started directly. In order to debug this project, you must consume it from an application project that creates a package and is marked as the startup project
```

#### Solution:

Right click your react-native-windows project name (i.e. AwesomeProject) and select "Set as StartUp Project"

#### Error:
(After trying to 'install missing features' on ReactNativeWebViewBridge when it is unavailable)
```
Object reference not set to an instance of an object
```

#### Solution:
Install the 10.0.14393.0 Windows 10 SDK in the Visual Studio Installer