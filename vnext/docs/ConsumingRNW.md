
# Consuming react native windows

## Installing React Native

- Install React Native command line interface using NPM:
  ```
  npm install -g react-native-cli
  ```
- Initialize your React Native project

  Next, [generate a React Native project](http://facebook.github.io/react-native/docs/getting-started.html#creating-a-new-application). In the directory you would like your React Native Windows project directory, run:
  ```
  react-native init <project name> --version 0.60.6
  ```
  Navigate into this newly created directory:
  ```
  cd <project name>
  ```

## Installing React Native Windows (vnext)

* Install the React Native Windows command line interface ([rnpm-plugin-windows](https://www.npmjs.com/package/rnpm-plugin-windows)).

  If you are using Yarn, run
  ```
  yarn add rnpm-plugin-windows
  ```
  Alternatively, if you are using NPM, run
  ```
  npm install --save rnpm-plugin-windows
  ```

* Next, initialize your React Native Windows `vnext` in the project directory. This step may take a while during first run due to dependency download.
  ```
  react-native windows --template vnext
  ```

## Running a React Native Windows App

*Note: Make sure a browser is launched and running before running a React Native Windows app.*

- Without Visual Studio

:exclamation: **We are seeing issues with msbuild with the below command. This is being investigated in [Issue 3263](https://github.com/microsoft/react-native-windows/issues/3263). Until this is fixed, please use the With Visual Studio option below.**

  In your React Native Windows project directory, run:
  ```
  react-native run-windows
  ```
  A new Command Prompt window will open with the React packager as well as a `react-native-windows` app. This step may take a while during first run since it involves building the entire project and all dependencies. You can now start developing! :tada:

- With Visual Studio

   - Open the solution file in the application folder in Visual Studio (e.g., `AwesomeProject/windows/AwesomeProject.sln`)
   - Select the `Debug` configuration and the `x64` platform from the combo box controls to the left of the `Run` button and underneath the `Team` and `Tools` menu item.
   - Run `yarn start` from your project directory, and wait for the React Native packager to report success.
   - Click the `Run` button to the right of the platform combo box control in VS, or select the `Debug`->`Start without Debugging` menu item. You now see your new app and Chrome should have loaded `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools. :tada:

- With VS Code
  - Open your applications folder in VS Code.
  - Install the [React Native Tools](https://marketplace.visualstudio.com/items?itemName=msjsdiag.vscode-react-native) plugin for VS Code.
  - Create a new file in the applications root directory, `.vscode/launch.json` and paste the following configuration:
  ```
  {
      "version": "0.2.0",
      "configurations": [
          {
              "name": "Debug Windows",
              "cwd": "${workspaceFolder}",
              "type": "reactnative",
              "request": "launch",
              "platform": "windows"
          }
      ]
  }
  ```
  - Press `F5` or navigate to the debug menu (alternatively press `Ctrl+Shift+D`) and in the Debug dropdown select "Debug Windows" and press the green arrow to run the application.

## Authoring Native Modules (vnext)

See [Native Modules and React Native Windows](NativeModules.md).

## Migrate project from old template
- Add Microsoft.UI.Xaml nuget
You need to manually add the nuget Microsoft.UI.Xaml 2.2.190917002 to your project

## Building a standalone React Native Windows App
Follow these steps to build a version of your app that you can install or publish to the store.  This version will package your bundle and assets into the appx package so you don't need to run Metro.

- Open the solution in Visual Studio
- Select the DebugBundle or ReleaseBundle configuration from the Configuration Manager dropdown.  DebugBundle is similar to Debug in that it adds more debugging info to the native code.  Use this if you want to debug the native code.  ReleaseBundle is similar to Release, you'll typically use this when producing a final package to publish to the store.
- Build the solution.  You can now launch without first launching Metro.
- If you want to build an appx package to share or publish, use the Project => Publish => Create App Packages... option.