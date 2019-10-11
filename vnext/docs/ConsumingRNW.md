
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

## Authoring Native Modules (vnext)

See [Native Modules and React Native Windows](NativeModules.md).
