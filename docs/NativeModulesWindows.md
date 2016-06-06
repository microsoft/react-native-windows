---
id: native-modules-windows
title: Native Modules
layout: docs
category: Guides (Windows)
permalink: docs/native-modules-windows.html
next: native-components-windows
---

Sometimes an app needs access to a platform API that React Native doesn't have a corresponding module for yet. Maybe you want to reuse some existing .NET code without having to reimplement it in JavaScript, or write some high performance, multi-threaded code such as for image processing, a database, or any number of advanced extensions.

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
    class DialogModule : ReactContextNativeModuleBase, ILifecycleEventListener
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