# Contributing Guidelines

This doc is a work in progress! Check back for updates.

## Running unit tests in Visual Studio
Before submitting a PR to React Native Windows, make sure the unit tests pass locally in Visual Studio.

#### But how?
![Run All Tests](img/RunTests.png)
1. In the top toolbar, click "Test"
2. Select "Run"
3. Select "All Tests"

#### Troubleshooting:
##### Error:
```
The build tools for v140 (Platform Toolset = 'v140') cannot be found. To build using the v140 build tools, please install v140 build tools.  Alternatively, you may upgrade to the current Visual Studio tools by selecting the Project menu or right-click the solution, and then selecting "Retarget solution".		

```
##### Solution:
Retarget the solution to v141:
1. Right click the ReactNative solution and click "Retarget Solution"
2. Make sure the Platform Toolset has Upgrade to v141 selected, and click "OK."
