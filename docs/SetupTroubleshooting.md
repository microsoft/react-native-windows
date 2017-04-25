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
