========================================================================
    Microsoft.ReactNative.Xaml Project Overview
========================================================================

This project demonstrates how to get started authoring Windows Runtime
classes directly with standard C++, using the Windows App SDK and
C++/WinRT packages to generate implementation headers from interface
(IDL) files.  The generated Windows Runtime component binary and WinMD
files should then be bundled with the app consuming them.

Steps:
1. Create an interface (IDL) file to define your Windows Runtime class,
    its default interface, and any other interfaces it implements.
2. Build the project once to generate module.g.cpp, module.h.cpp, and
    implementation templates under the "Generated Files" folder, as
    well as skeleton class definitions under "Generated Files\sources".
3. Use the skeleton class definitions for reference to implement your
    Windows Runtime classes.

========================================================================
Learn more about Windows App SDK here:
https://docs.microsoft.com/windows/apps/windows-app-sdk/
Learn more about WinUI3 here:
https://docs.microsoft.com/windows/apps/winui/winui3/
Learn more about C++/WinRT here:
http://aka.ms/cppwinrt/
========================================================================
