
# Building React-Native-Windows

- Make sure you have installed [dependencies](./GettingStarted.md#dependencies)
- Install [Git](https://git-scm.com/download/win) if you don't have it installed in your development machine

## Build Steps
* Clone the repo
    ```cmd
    git clone https://github.com/microsoft/react-native-windows.git
    cd react-native-windows
    ```

* Install dependencies, and run JS build steps
    ```cmd
    yarn
    yarn build
    ```

# Running the Playground app
  There are two ways to run the app.  In a fully managed easy way, or by manually running all the required steps:

## Automatic
The playground app can be run in a completely automatic way by using `react-native run-windows`.

If you haven't already, install the react-native-cli  (One time only!)
```cmd
npm install -g react-native-cli
```

Then 

```cmd
cd packages\playground
react-native run-windows
```

## Manual

### Run the bundle server:
```cmd
cd packages\playground
yarn start
```

### Build solution

  * Using MSBuild
    ```cmd
    MSBuild.exe [/p:Platform=$(TargetPlatform)] [/p:Configuration=$(TargetConfiguration)]
    ```

  * Using Visual Studio IDE
    1. Open `Playground.sln`.
    2. Set your `Platform` to `x86` or `x64` and `Configuration ` to `Debug`.
    3. Select `Project / Build Solution (Ctrl+Shift+B)`

### Ensure debugger is running
Open Chrome and navigate to `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools.

### Run the app
* In Visual Studio, set Playground as the StartUp Project.

* Run project (`F5` or `Debug / Start Debugging`).

You now see your new app and Chrome should have loaded `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools.

### Load a sample bundle
* Press the "Load" button on the left side of the Windows 10 application window that appears.

 The selected React Native component (defaulted to `Bootstrap`) should get loaded in the bottom of the application window

 Try out the various samples by selecting from the comboboxes at the top of the application window and pressing "Load".
