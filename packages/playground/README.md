# Playground

Playground is a sample standalone application that shows usage of React Native for Windows vNext with several test scenarios. 

## Launching the app
1. Make sure your development machine has been set up with all the system requirements and dependencies mentioned [here](https://github.com/microsoft/react-native-windows/blob/master/vnext/docs/GettingStarted.md). Make sure a browser is launched and running.

2. Open a command prompt in Administrator mode, Navigate to the root folder and install React Native

`npm install -g react-native-cli`

3. Install React Native for Windows

`yarn add rnpm-plugin-windows`
`yarn install`

4. Ensure packages are built

`yarn build`

5. Navigate to the playground folder

`cd packages\playground`

6. Run the app

`react-native run-windows`

This command will build and deploy the application along with launching Metro bundler and the dev tools. 

## Using the app

You can find several sample .tsx files under the `/playground/samples` folder. You can load any of these files from the drop down at the top of the app, ensure the right App/component name has been picked for the sample on the drop down against "App Name". Click `Load` to load the .tsx file to the bottom pane. 

You can edit the .tsx files for live reload. You can also add multiple React Native for Windows contents within the same app by clicking `+Pane` and loading a different .tsx file into each. 

Use the toggles for Web debugger, Live Reload and Reuse Instance to change/test your scenarios.

## Editing the app

You can access and edit the .tsx files in the `/playground/samples` folder. Live reload should work as expected while editing the Typescript files.

You can also launch `/playground/windows/playground.sln` solution file in Visual Studio and edit the native C++ code in the Playground Project. You will have to re-launch the app with the above steps if edits are made to the native app code.
