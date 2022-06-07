# Playground

Playground is a sample standalone application that allows testing of various JS files, including RNTester.

## Using the app

You can find several sample .tsx files under the `/playground/samples` folder. You can load any of these files from the drop down at the top of the app, ensure the right App/component name has been picked for the sample on the drop down against "App Name". Click `Load` to load the .tsx file to the bottom pane.

Use the UI at the top of the playground application to modify the instance settings, then hit `Load` to run the instance.

## How to remote debug Playground

1. On your dev machine, open `packages/playground/windows/Playground.sln`
2. On your dev machine, run the metro bundler by doing `yarn start --host devMachineName`
3. On your target machine, run MSVSMon. This can be downloaded from [here](https://visualstudio.microsoft.com/downloads/#remote-tools-for-visual-studio-2019).
   1. I haven’t been able to figure out authentication so once you run msvsmon, click on `Options` and select no authentication.
4. Right click on the Playground project, Properties, Debugging
   1. Select Remote debugging, and enter the IP of the debug target
   2. In the command line arguments textbox, enter `devMachineName:8081`
   3. In the protocol dropdown, select no authentication
5. F5! This will build your playground app, and deploy to the target machine.
6. On the target, press `Load`. This will communicate back to the dev machine bundler and launch the web debugger on the dev machine, and serve the pages back to the app running in the target.
   You need an updated version of the bundler in order for remote debugging to work properly: [PR here](https://github.com/react-native-community/cli/pull/829)

# Playground-win32

Playground-win32 is a sample of an RNW application using XAML Islands that allows testing of various JS files, including RNTester.

## Using the app

You can find several sample .tsx files under the `/playground/samples` folder. You can load any of these files by selecting the "File" menu, then "Open Javascript File", and then clicking on the sample you wish to open.

# Launching and Editing
## Launching the app

1. Make sure your development machine has been set up with all the system requirements and dependencies mentioned [here](https://microsoft.github.io/react-native-windows/docs/next/rnw-dependencies). Make sure a browser is launched and running.

1. Install Packages with-in the repo

`yarn`

1. Ensure packages are built

`yarn build`

1. Navigate to the `playground` folder

`cd packages\playground`

1. Start Metro

`yarn start`

1. Open the app solution file in Visual Studio either `windows\playground.sln` or `windows\playground-win32.sln` 

1. Set the build configuration to "Debug" and Platform to "x86" or "x64"

1. Hit "Local Windows Debugger"

This will build and deploy the application and the dev tools.

## Editing the app

You can access and edit the .tsx files in the `/playground/samples` folder. Fast Refresh should work as expected while editing the Typescript files.

You can also launch the app solution file in Visual Studio and edit the native C++ code in the Playground Project. You will have to re-launch the app with the above steps if edits are made to the native app code.
