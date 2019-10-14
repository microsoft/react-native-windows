# HeadlessJS
HeadlessJS provides a mechanism to run background work when your application is not in the foreground.  
This is a complimentary document to the [react-native HeadlessJS documentation](https://facebook.github.io/react-native/docs/0.60/headless-js-android)

## UWP background tasks 
HeadlessJS support in react-native-windows enables starting a react instance without an app window.  
This means that you can write a code in javascript, and take advantage of [UWP background tasks](https://docs.microsoft.com/en-us/windows/uwp/launch-resume/support-your-app-with-background-tasks) to request background execution.

### In-Process vs Out-of-Process
HeadlessJS code which is executed with a UWP background task can be configured to run either in-process or out-of-process.  
[Guidelines for background tasks](https://docs.microsoft.com/en-us/windows/uwp/launch-resume/guidelines-for-background-tasks) provides a general overview of running background tasks in-process vs out-of-process.  
The playground demonstrates Registering and running and in-process background task.  
Running an out-of-process background task requires making changes to the app manifest.  
See [Create and register an out-of-process background task](https://docs.microsoft.com/en-us/windows/uwp/launch-resume/create-and-register-a-background-task) for details.

## Transition React Instance from background to foreground
A React Instance that is created for executing an in-process background task can be transitioned to be used in the foreground. **NOTE: This is only possible for in-process background tasks as a foreground app and a out-of-process background task do not share memory, therefor the React Instance cannot be re-used**

## Debugging background task execution
The MSDN page [Debug a background task](https://docs.microsoft.com/en-us/windows/uwp/launch-resume/debug-a-background-task) provides information that is relevant for debugging background tasks in general.  
One scenario that is not covered by this page is debugging an in-process background task that executes before the app is launched.  
To Debug running an in-process background task set your debugger to [disable automatic app startup](https://docs.microsoft.com/en-us/visualstudio/debugger/start-a-debugging-session-for-a-store-app-in-visual-studio-vb-csharp-cpp-and-xaml?view=vs-2019#BKMK_Start_debugging__F5__but_delay_the_app_start)
If Visual Studio drop-down does not show your task, you can use [PLMDebug](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/plmdebug) to trigger your task. [RunHeadlessJsTask.ps1](..\playground\playground\RunHeadlessJsTask.ps1) provides an example of using PLMDebug to trigger a background task.  
Once the native code is triggered to run the background task the web debugger will work as normal to debug your javascript executing with headlessJS.

## Playground example
[The playground](..\packages\playground) app demonstrates how an in-process background task can be used to fetch data displayed in the UI. The React Instance, and the data displayed is re-used from the background task.  
You can try out running a background task and transitioning to the foreground using the [headlessJS sample](..\packages\playground\Samples\headlessjs.tsx)

1. Launch the playground.
2. Select headlessjs from the sample dropdown, and click load.
3. Click 'register Headless Task'
4. Close the playground.
5. Use the [RunHeadlessJsTask.ps1](..\playground\playground\RunHeadlessJsTask.ps1) powershell script to trigger the background task.
6. Run the playground app from the start menu.
7. Click load (the script headlessjs should already be selected.)

You should now see the ToDos which were fetched by the background task rendered