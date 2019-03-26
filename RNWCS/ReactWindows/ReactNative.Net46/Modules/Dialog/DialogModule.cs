// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.Json;
using System.Windows;

namespace ReactNative.Modules.Dialog
{
    class DialogModule : ReactContextNativeModuleBase
    {
        public DialogModule(ReactContext reactContext)
            : base(reactContext, new DispatcherActionQueue(reactContext.HandleException))
        {
        }

        public override string Name
        {
            get
            {
                return "DialogManagerWindows";
            }
        }

        public override JObject ModuleConstants
        {
            get
            {
                return new JObject
                {
                    { DialogModuleHelper.ActionButtonClicked, DialogModuleHelper.ActionButtonClicked },
                    { DialogModuleHelper.ActionDismissed, DialogModuleHelper.ActionDismissed },
                    { DialogModuleHelper.KeyButtonPositive, DialogModuleHelper.KeyButtonPositiveValue },
                    { DialogModuleHelper.KeyButtonNegative, DialogModuleHelper.KeyButtonNegativeValue },
                };
            }
        }

        [ReactMethod]
        public void showAlert(
            JObject config,
            ICallback errorCallback,
            ICallback actionCallback)
        {
            var message = config.Value<string>("message") ?? "";
            var title = config.Value<string>("title") ?? "";
            bool containsTitle = config.ContainsKey("title");
            bool containsPositive = config.ContainsKey(DialogModuleHelper.KeyButtonPositive);
            bool containsNegative = config.ContainsKey(DialogModuleHelper.KeyButtonNegative);

            if (containsPositive && containsNegative)
            {
                var result = MessageBox.Show(message, title, MessageBoxButton.OKCancel);
                if (result == MessageBoxResult.OK)
                {
                    actionCallback.Invoke(DialogModuleHelper.ActionButtonClicked, DialogModuleHelper.KeyButtonPositiveValue);
                }
                else
                {
                    actionCallback.Invoke(DialogModuleHelper.ActionButtonClicked, DialogModuleHelper.KeyButtonNegativeValue);
                }
            }
            else if (containsPositive)
            {
                var result = MessageBox.Show(message, title, MessageBoxButton.OK);
                if (result == MessageBoxResult.OK)
                {
                    actionCallback.Invoke(DialogModuleHelper.ActionButtonClicked, DialogModuleHelper.KeyButtonPositiveValue);
                }
            }
            else if (containsTitle)
            {
                MessageBox.Show(message, title);
            }
            else
            {
                MessageBox.Show(message);
            }
        }
    }
}
