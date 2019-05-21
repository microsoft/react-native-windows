# Contributing Guidelines

This doc is a work in progress! Check back for updates.

Check out the React Native Windows code itself and install npm dependencies

```
git clone --recursive https://github.com/microsoft/react-native-windows.git
cd react-native-windows\current
npm install
```

Each pull request has the unit tests, code analysis, and a [Winium](https://github.com/2gis/Winium) integration test run in the AppVeyor CI service. To shorten the feedback cycle, please be sure to [run the unit tests in Visual Studio](#running-unit-tests-in-visual-studio) and verify they are passing before submitting pull requests. For extra credit, [verify the examples in RNTester](#using-rntester) continue to work properly.

## Running unit tests in Visual Studio
Before submitting a PR to React Native Windows, make sure the unit tests pass locally in Visual Studio.

#### But how?
![Run All Tests](img/RunTests.png)
1. Open ReactNative solution file (react-native-windows\current\ReactWindows\ReactNative.sln)
2. Set the Solution Configuration to "Debug" and the Solution Platforms to "x86"
3. In the top toolbar, click "Test"
4. Select "Run"
5. Select "All Tests"

#### Troubleshooting:
##### Error:
```
The build tools for v140 (Platform Toolset = 'v140') cannot be found. To build using the v140 build tools, please install v140 build tools.  Alternatively, you may upgrade to the current Visual Studio tools by selecting the Project menu or right-click the solution, and then selecting "Retarget solution".	

```
##### Solution:
Retarget the solution to v141:
1. Right click the ReactNative solution and click "Retarget Solution"
2. Make sure the Platform Toolset has Upgrade to v141 selected, and click "OK."


## Using RNTester
RNTester is a React Native Windows app that demonstrates the implemented views and modules of React Native Windows.

You can use it to test your changes to React Native Windows by making sure your changes haven't broken the views and modules.

#### But How?
Before starting make sure you have run `npm install` in the react-native-windows directory. Additionally, make sure the RNTester submodule is up to date by running `git pull --recurse-submodules` from the react-native-windows directory.

Use Visual Studio 2017 or higher, with the Windows 10 SDK 10.0.14393 or higher.

1. Open the RNTester solution file (react-native-windows\RNTester\RNTester.sln) in Visual Studio
2. Set RNTesterApp as the StartUp project
3. Set the Solution Configuration to "Debug" and the Solution Platforms to "x86"
4. Run the project on the local machine by clicking the run button or by pressing F5

A Windows app will open, and it may have a red box dialog stating `Unable to download JS bundle. Did you forget to start the development server or connect your device?` We still need to start the dev server!

5. In your command line, make sure that you are in the react-native-windows directory and run then `react-native start`
6. Reload the bundle in the app by clicking "Reload JavaScript" or by pressing Ctrl+R

The packager server will bundle the application JavaScript, and you can now use RNTester to test your changes to the repository.
