# Playground-composition

Playground is a sample standalone application that allows testing of various JS files, including RNTester.

# Launching and Editing
## Launching the app

1. Clone [this (react-native-windows)](https://github.com/microsoft/react-native-windows) repository (or your own fork of it, if you intend on submitting PRs)

1. Make sure your development machine has been set up with all the system requirements and dependencies.  These can be verified/installed by running `.\vnext\Scripts\rnw-dependencies.ps1 -Tags 'rnwDev'` from an elevated powershell prompt.
    > **Note:** The script defaults to Visual Studio Community Edition. If you're using (or want to use) Visual Studio Enterprise Edition, run `.\vnext\Scripts\rnw-dependencies.ps1 -Tags 'rnwDev' -Enterprise`.

1. Install JavaScript dependencies within the repo by running `yarn` at the root of the repo.

1. Navigate to the `playground` folder  (`cd packages\playground`)

1. Start Metro from within **package\playground** by running `yarn start` - This dev process must be running for the application to be able to load javascript files.

1. Open the app solution file in Visual Studio `packages\playground\windows\playground-composition.sln`

1. Ensure **playground-composition.Package** project is set as startup project

1. Hit F5


## Using the app

### Configure lifted vs system compostion

Currently the application will run on either lifted composition using WinAppSDK, or using system compostion.  -- You can switch between the two from the `file -> Options` dialog.  Note this must be done before you open a javascript file from the playground app. You have to restart the application to switch between the two modes.

You can find several sample .tsx files under the `/playground/samples` folder. You can load any of these files from `file -> Open JavaScript File` in the menu bar at the top of the app.  The `Samples\rntester` javascript file is the main RNTester application provided by react-native, which has been extended with additional test pages.

### Interesting test pages for Islands work

In the `Samples\rntester` app there are a couple of test pages that host XAML controls within composition.  These are labeled `Fabric Native Component` and `Fabric Native Component Yoga`.  If you run these test pages with lifted composition you will get a "Unimplemented Component" placeholder in place of the XAML island - since Xaml islands are not hostable within lifted composition today.


## How to debug Playground

Native code can be debugged as normal through Visual Studio.  

To debug the javascript code: 

1. enable "Direct Debugger" from the `file -> options` dialog.
1. Navigate to `edge://inspect` within Microsoft Edge.  
1. Ensure `localhost:9229` is in the list of network targets within the "Discover Network Targets" configuration.
1. A remote target for "Hermes" should show up.  Click on "inspect"
1. In the Microsoft Edge Devtools window that pops up, you can hit Ctrl+p and type the name of a file to set a break point in.
