# Getting Started

## Installing React Native and its dependencies

This is a targeted summary of setup React Native setup for specifically React Native Windows. See the [React Native Getting Started Guide](http://facebook.github.io/react-native/docs/getting-started.html) for full details.

*Note*: The Linux commands in the offical React Native Getting Started Guide should work on the Windows Subsystem for Linux bash shell, but this scenario is not currently officially supported. PowerShell or the Visual Studio Developer Command Prompt are recommended.

#### Dependencies: Node

Install the dependencies [specified by React Native](http://facebook.github.io/react-native/docs/getting-started.html#node-python2-jdk). Specifically, make sure a recent version of Node is installed.

[Chocolatey](https://chocolatey.org/) is the React Native recommended installation method. On an elevated Command Prompt, run:

```
choco install nodejs
```

#### React Native CLI

Next, we need the React Native command line interface. Install using NPM:
```
npm install -g react-native-cli
```

#### Initialize your project

Next, [generate a React Native project](http://facebook.github.io/react-native/docs/getting-started.html#creating-a-new-application). In the directory you would like your React Native Windows project directory, run:
```
react-native init <project name> --version 0.59.10
```
Navigate into this newly created directory:
```
cd <project name>
```

## Installing React Native Windows

#### React Native Windows CLI

Now we need to install the React Native Windows command line interface ([rnpm-plugin-windows](https://www.npmjs.com/package/rnpm-plugin-windows)).

If you haven't already done so, navigate into your React Native project directory.

If you are using NPM, run
```
npm install --save-dev rnpm-plugin-windows
```

If you are using Yarn, run
```
yarn add rnpm-plugin-windows --dev
```

#### Initialize your project

Initialize your React Native Windows project in the project directory by running:
```
react-native windows
```

#### *Note on Requirements*

Make sure you have installed all [requirements](https://github.com/microsoft/react-native-windows#system-requirements) for React Native Windows. These requirements are necessary to build and run your application.

#### *Note On Expo*

React Native Windows is not currently supported by Expo. If you have a React Native project created by [create-react-native-app](https://github.com/react-community/create-react-native-app) on which you would like to develop React Native Windows, [you can eject your app from Expo](https://github.com/react-community/create-react-native-app/blob/master/EJECTING.md).

## Running a React Native Windows App

#### With Visual Studio

- Open the solution file in the application folder in Visual Studio (e.g., `AwesomeProject/windows/AwesomeProject.sln`)
	- *Note*: If this is your first time doing UWP development on the computer you're using, you may be asked to install additional UWP tooling. After opening the solution, right click the Solution in the `Solution Explorer` and select the option labeled `Install Missing Components`. You may have to shutdown Visual Studio to continue the installation.
- *Note*: If you are using Visual Studio 2017, you will need to retarget the ChakraBridge project. Right click the ChakraBridge project:
    - If there is not a retarget option:
        - Select "Reload", and the Visual Studio installer will open.
        - Make sure to close Visual Studio before installing
        - Make sure the 10.0.14393.0 Windows 10 SDK is selected along with any other preselected components, and click `Modify` in the installer to install the components
        - Retargeting the ChakraBridge project should be possible, so proceed to the next instructions regarding if there is a retarget option
    - If there is a retarget option:
        - Retarget by right clicking on the ChakraBridge project in the Solution Explorer and selecting `Retarget Projects` and pressing `OK` on the popup dialog (Platform Toolset should say `Upgrade to v141` in the dialog).
- Select the `Debug` configuration and the `x64` platform from the combo box controls to the left of the `Run` button and underneath the `Team` and `Tools` menu item.
	- *Note:* If you are running on, or targeting, an x86 platform select `x86` instead. If you are deploying to Windows 10 Phone, select `ARM`.
- Run `react-native start` from your project directory, and wait for the React Native packager to report success.
- Click the `Run` button to the right of the platform combo box control, or select the `Debug`->`Start without Debugging` menu item. You now see your new app! :tada:

*Note:* You should **only** modify the project and source files for your app (e.g. `AwesomeProject`). The files for the "ReactNative" and other projects shown in the Visual Studio solution are in the `node_modules` directory (which will not be committed to your source repository since it is ignored in `.gitignore`). Any changes to files in `node_modules` will be overwritten when doing an `npm install` or `npm update`. If you need to add a new native module or override some React Native behavior, see [Extending React Native](http://github.com/microsoft/react-native-windows#extending-react-native)

#### Without Visual Studio

In your React Native Windows project directory, run:
```
react-native run-windows
```

A new Command Prompt window will open with the React packager as well as a React Native Windows app. You can now start developing! :tada:

## E2E Test
Please refer to [Author and Run E2E Test for React Native Windows](E2ETest.md)

## Additional Resources

- For more information about how to deploy to Windows simulators/devices, and how to create a bundle for Release, view [RunningOnDeviceWindows](RunningOnDeviceWindows.md)
- Learn more about various [tools for debugging React Native](http://facebook.github.io/react-native/docs/debugging.html).

## Setup Troubleshooting

If you happen to run into issues while setting up React Native Windows on your machine, check out the [Setup Troubleshooting](SetupTroubleshooting.md) documentation.
