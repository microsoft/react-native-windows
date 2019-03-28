---
id: native-modules-windows
title: Native Modules
layout: docs
category: Guides (Windows)
permalink: docs/native-modules-windows.html
next: native-components-windows
---

Sometimes an app needs access to a platform API that React Native doesn't have a corresponding module for yet. Maybe you want to reuse some existing .NET code without having to reimplement it in JavaScript, or write some high performance, multi-threaded code for image processing, a database, or any number of advanced extensions.

React Native was designed such that it is possible for you to write real native code and have access to the full power of the platform. This is a more advanced feature and we don't expect it to be part of the usual development process, however it is essential that it exists. If React Native doesn't support a native feature that you need, you should be able to build it yourself.

## The Dialog Module ##

This guide will use the Dialog example. Let's say we would like to be able to create a dialog message from JavaScript.

We start by creating a native module. A native module is a C# class that usually extends the `ReactContextNativeModuleBase` class and implements the functionality required by the JavaScript. Our goal here is to be able to write the following in JavaScript to launch a dialog module.

```js
const DialogModule = require('NativeModules').DialogManagerWindows;

DialogModule.showAlert(
  { title: 'Sample alert', message: 'This is just a test' },
  (errorMsg) => console.warn(`Error ocurred: ${errorMsg}`),
  (action, buttonKey) => {
    console.log(`Action key ${action}`);
    console.log(`Button key ${buttonKey}`);
  });
```

Below is the C# code required to set up the class.

```csharp
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.Popups;

namespace ReactNative.Modules.Dialog
{
    public class DialogModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private const string ActionButtonClicked = "buttonClicked";
        private const string ActionDismissed = "dismissed";

        private const string KeyButtonPositive = "buttonPositive";
        private const string KeyButtonNegative = "buttonNegative";

        private const int KeyButtonPositiveValue = 0;
        private const int KeyButtonNegativeValue = 1;

        private MessageDialog _pendingDialog;
        private bool _isInForeground;

        public DialogModule(ReactContext reactContext)
            : base(reactContext)
        {
        }
    }
}
```

The `ReactContextNativeModuleBase` abstract base class requires that a property with a getter called `Name` is implemented. The purpose of this method is to return the string name of the NativeModule which represents this class in JavaScript. So here we will call this `DialogManagerWindows` so that we can access it through `React.NativeModules.DialogManagerWindows` in JavaScript.

```csharp
public override string Name
{
    get
    {
        return "DialogManagerWindows";
    }
}
```

An optional property with a getter called `Constants` returns the constant values exposed to JavaScript. Its implementation is not required but is very useful to key pre-defined values that need to be communicated from JavaScript to .NET in sync.

```csharp
public override IReadOnlyDictionary<string, object> Constants
{
    get
    {
        return new Dictionary<string, object>
        {
            { ActionButtonClicked, ActionButtonClicked },
            { ActionDismissed, ActionDismissed },
            { KeyButtonPositive, KeyButtonPositiveValue },
            { KeyButtonNegative, KeyButtonNegativeValue },
        };
    }
}
```

To expose a method to JavaScript a .NET method must be annotated using the `[ReactMethod]` attribute. The return type of bridge methods is always `void`. The React Native bridge is asynchronous, so the only way to pass a result to JavaScript is by using callbacks or emitting events (see below).

```csharp
[ReactMethod]
public void showAlert(
    JObject config,
    ICallback errorCallback,
    ICallback actionCallback)
{
    var message = config.Value<string>("message") ?? "";
    var messageDialog = new MessageDialog(message)
    {
        Title = config.Value<string>("title"),
    };

    if (config.ContainsKey(KeyButtonPositive))
    {
        messageDialog.Commands.Add(new UICommand
        {
            Label = config.Value<string>(KeyButtonPositive),
            Id = KeyButtonPositiveValue,
            Invoked = target => OnInvoked(target, actionCallback),
        });
    }

    if (config.ContainsKey(KeyButtonNegative))
    {
        messageDialog.Commands.Add(new UICommand
        {
            Label = config.Value<string>(KeyButtonNegative),
            Id = KeyButtonNegativeValue,
            Invoked = target => OnInvoked(target, actionCallback),
        });
    }

    RunOnDispatcher(async () =>
    {
        if (_isInForeground)
        {
            await messageDialog.ShowAsync();
        }
        else
        {
            _pendingDialog = messageDialog;
        }
    });
}

private void OnInvoked(IUICommand target, ICallback callback)
{
    callback.Invoke(ActionButtonClicked, target.Id);
}
```

### Argument Types

This project uses [Newtonsoft Json.NET](http://www.newtonsoft.com/json) to provide interoperability with JavaScript types. The parameter types of a `[ReactMethod]` may be of any type that can be deserialized using Json.NET. Be aware that the use of composite types such as arrays, generics, and user-defined classes may not be supported out-of-the-box with [.NET Native](https://msdn.microsoft.com/en-us/library/dn584397.aspx) pre-compilation, and you may need add information to the [runtime directives (rd.xml)](https://msdn.microsoft.com/en-us/library/dn600639.aspx) file, or just manually deconstruct the JSON by using [`JArray`](http://www.newtonsoft.com/json/help/html/T_Newtonsoft_Json_Linq_JArray.htm) or [`JObject`](http://www.newtonsoft.com/json/help/html/T_Newtonsoft_Json_Linq_JObject.htm) as the parameter types.

### Register the Module

The last step within .NET is to register the Module; this happens in the `CreateNativeModules` of your apps package. If a module is not registered it will not be available from JavaScript.

```csharp
public class AnExampleReactPackage : IReactPackage
{
    public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
    {
        return new List<INativeModule>
        {
            new DialogModule(reactContext)
        };
    }

    public IReadOnlyList<Type> CreateJavaScriptModulesConfig()
    {
        return new List<Type>(0);
    }

    public IReadOnlyList<IViewManager> CreateViewManagers(
        ReactContext reactContext)
    {
        return new List<IViewManager>(0);
    }
}
```

The package needs to be provided in the `Packages` property getter of the `MainPage.cs` file. This file exists under the `windows` folder in your react-native application directory. The path to this file is: `windows/your-app-name/MainPage.cs`.

```csharp
public override List<IReactPackage> Packages
{
    get
    {
        return new List<IReactPackage>
        {
            new MainReactPackage(),
            new AnExampleReactPackage()
        };
    }
}
```

To make it simpler to access your new functionality from JavaScript, it is common to wrap the native module in a JavaScript module. This is not necessary but saves the consumers of your library the need to pull it off of `NativeModules` each time. This JavaScript file also becomes a good location for you to add any JavaScript side functionality.

```js
'use strict';
/**
 * This exposes the native DialogModuleWindows module as a JS module. This has a
 * function 'showAlert' which takes the following parameters:
 *
 * 1. config message: An object with the title and message as strings
 * 2. error callback: A callback with an error message if one occurred.
 * 3. success callback: A callback with both the action and which button was pressed
 */
import { NativeModules } from 'react-native';
module.exports = NativeModules.DialogModuleWindows;
```

Now, from your other JavaScript file you can call the method like this:

```js
import DialogManager from './DialogManagerWindows';

DialogModule.showAlert(
  { title: 'Sample alert', message: 'This is just a test' },
  (errorMsg) => console.warn(`Error ocurred: ${errorMsg}`),
  (action, buttonKey) => {
    console.log(`Action key ${action}`);
    console.log(`Button key ${buttonKey}`);
  });
```

## Beyond the Basics ##

### Callbacks ###

Native modules also support a special kind of argument - a callback. In most cases it is used to provide the function call result to JavaScript.

```csharp
[ReactMethod]
public void showAlert(
    JObject config,
    ICallback errorCallback,
    ICallback actionCallback)
{
    var message = config.Value<string>("message") ?? "";
    var messageDialog = new MessageDialog(message)
    {
        Title = config.Value<string>("title"),
    };

    if (config.ContainsKey(KeyButtonPositive))
    {
        messageDialog.Commands.Add(new UICommand
        {
            Label = config.Value<string>(KeyButtonPositive),
            Id = KeyButtonPositiveValue,
            Invoked = target => OnInvoked(target, actionCallback),
        });
    }

    if (config.ContainsKey(KeyButtonNegative))
    {
        messageDialog.Commands.Add(new UICommand
        {
            Label = config.Value<string>(KeyButtonNegative),
            Id = KeyButtonNegativeValue,
            Invoked = target => OnInvoked(target, actionCallback),
        });
    }

    RunOnDispatcher(async () =>
    {
        if (_isInForeground)
        {
            await messageDialog.ShowAsync();
        }
        else
        {
            _pendingDialog = messageDialog;
        }
    });
}
```

This method would be accessed in JavaScript using:

```js
const DialogModule = require('NativeModules').DialogManagerWindows;

DialogModule.showAlert(
  { title: 'Sample alert', message: 'This is just a test' },
  (errorMsg) => console.warn(`Error ocurred: ${errorMsg}`),
  (action, buttonKey) => {
    console.log(`Action key ${action}`);
    console.log(`Button key ${buttonKey}`);
  });
```

A native module is supposed to invoke its callback only once. It can, however, store the callback and invoke it later.

It is very important to highlight that the callback is not invoked immediately after the native function completes - remember that bridge communication is asynchronous, and this too is tied to the run loop.

### Promises

Native modules can also fulfill a promise, which can simplify your code, especially when using ES2016's `async/await` syntax. When the last parameter of a bridged native method is a `IPromise`, its corresponding JS method will return a JS Promise object.

```csharp
[ReactMethod]
public async void canOpenURL(string url, IPromise promise)
{
    if (url == null)
    {
        promise.Reject(new ArgumentNullException(nameof(url)));
        return;
    }

    var uri = default(Uri);
    if (!Uri.TryCreate(url, UriKind.Absolute, out uri))
    {
        promise.Reject(new ArgumentException($"URL argument '{uri}' is not valid."));
        return;
    }

    try
    {
        var support = await Launcher
            .QueryUriSupportAsync(uri, LaunchQuerySupportType.Uri)
            .AsTask().ConfigureAwait(false);
        promise.Resolve(support == LaunchQuerySupportStatus.Available);
    }
    catch (Exception ex)
    {
        promise.Reject(new InvalidOperationException(
            $"Could not check if URL '{url}' can be opened.", ex));
    }
}
```
The JavaScript counterpart of this method returns a Promise. This means you can use the `await` keyword within an async function to call it and wait for its result:

```js
async function canOpenUrl(url) {
  try {
    var canOpen = await Launcher.canOpenUrl(url);
    console.log(canOpen);
  } catch (e) {
    console.error(e);
  }
}

canOpenUrl('http://foo.bar');
```

### Threading

Native modules should not have any assumptions about what thread they are being called on, as the current assignment is subject to change in the future. If a blocking call is required, the heavy work should be dispatched to an internally managed worker thread, and any callbacks distributed from there.

### Sending Events to JavaScript

Native modules can signal events to JavaScript without being invoked directly. The easiest way to do this is to use the `RCTDeviceEventEmitter` which can be obtained from the `Context` as in the code snippet below.

```csharp
private void SendEvent(string eventName, JObject parameters)
{
    Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
        .emit(eventName, parameters);
}
...
SendEvent("toggleElementInspector", null);
```

JavaScript modules can then register to receive events by `addListener` from `NativeEventEmitter` wrapped module.

```js
var NativeEventEmitter = require('NativeEventEmitter');
var DialogModule = require('NativeModules').DialogManagerWindows;
var DialogEventEmitter = new NativeEventEmitter(DialogModule);
...

var AppContainer = React.createClass({

  componentWillMount: function() {
    ...
    DialogEventEmitter.addListener(
      'toggleElementInspector',
      this.toggleElementInspector
    );
    ...
  },
  toggleElementInspector: function() {
    this.setState({
      inspectorVisible: !this.state.inspectorVisible,
      rootNodeHandle: ReactNative.findNodeHandle(this.refs.main),
    });
  },
```

### Listening to Life Cycle Events

Listening to the application's life cycle events such as `OnSuspend`, `OnResume` etc. may be important to your application. In order to listen to these events, the module must implement the `ILifecycleEventListener` interface. Then, you need to register a listener in the module's `Initialize` method.

```csharp
public override void Initialize()
{
    Context.AddLifecycleEventListener(this);
}
```

Now you can listen to the application's life cycle events by implementing the following methods:

```csharp
public void OnSuspend()
{
    // Application Suspending
}

public async void OnResume()
{
    // Application Resuming
}

public void OnDestroy()
{
    // Application Terminating
}
```
