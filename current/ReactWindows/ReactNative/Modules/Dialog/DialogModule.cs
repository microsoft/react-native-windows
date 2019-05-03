// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Json;
using ReactNative.UIManager;
using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.Popups;

namespace ReactNative.Modules.Dialog
{
    class DialogModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private class MessageDialogInfo
        {
            public MessageDialog MessageDialog { set; get; }

            public int? RootViewHint { set; get; }

            public Action<string> ErrorCallback { set; get; }
        }

        private MessageDialogInfo _pendingDialog;
        private bool _isInForeground;

        public DialogModule(ReactContext reactContext)
            : base(reactContext)
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

        public override void Initialize()
        {
            Context.AddLifecycleEventListener(this);
        }

        public void OnSuspend()
        {
            _isInForeground = false;   
        }

        public void OnResume()
        {
            _isInForeground = true;

            var pendingDialog = _pendingDialog;
            _pendingDialog = null;
            if (pendingDialog != null)
            {
                ShowDialog(pendingDialog);
            }
        }

        public void OnDestroy()
        {
        }

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

            MessageDialogInfo dialogInfo = new MessageDialogInfo
            {
                MessageDialog = messageDialog,
                ErrorCallback = (string error) => errorCallback.Invoke(error)
            };

            if (config.ContainsKey(DialogModuleHelper.RootViewHint))
            {
                dialogInfo.RootViewHint = config.Value<int>(DialogModuleHelper.RootViewHint);
            }

            uint commandIndex = 0;
            if (config.ContainsKey(DialogModuleHelper.KeyButtonPositive))
            {
                messageDialog.Commands.Add(new UICommand
                {
                    Label = config.Value<string>(DialogModuleHelper.KeyButtonPositive),
                    Id = DialogModuleHelper.KeyButtonPositiveValue,
                    Invoked = target => OnInvoked(target, actionCallback),
                });
                commandIndex ++;
            }

            if (config.ContainsKey(DialogModuleHelper.KeyButtonNegative))
            {
                messageDialog.Commands.Add(new UICommand
                {
                    Label = config.Value<string>(DialogModuleHelper.KeyButtonNegative),
                    Id = DialogModuleHelper.KeyButtonNegativeValue,
                    Invoked = target => OnInvoked(target, actionCallback),
                });

                // Use this command for Escape (we don't use the DialogModuleHelper.ActionDismissed since
                // it's hard to detect the condition
                messageDialog.CancelCommandIndex = commandIndex;
                commandIndex++;
            }

            DispatcherHelpers.RunOnDispatcher(() =>
            {
                if (_isInForeground)
                {
                    ShowDialog(dialogInfo);
                }
                else
                {
                    _pendingDialog = dialogInfo;
                }
            });
        }

        private void OnInvoked(IUICommand target, ICallback callback)
        {
            callback.Invoke(DialogModuleHelper.ActionButtonClicked, target.Id);
        }

        private void ShowDialog(MessageDialogInfo dialog)
        {
            var uiManager = Context.GetNativeModule<UIManagerModule>();
            try
            {
                uiManager.AddUIBlock(new UIBlock(async () =>
                {
                    if (CoreApplication.GetCurrentView().CoreWindow == null)
                    {
                        // View has no CoreWindow, won't be able to display a dialog box
                        dialog.ErrorCallback($"Alert failed: CoreApplicationView corresponding to RootViewHint {dialog.RootViewHint} has no CoreWindow");
                        return;
                    }

                    await dialog.MessageDialog.ShowAsync();
                }
                ), dialog.RootViewHint);
            }
            catch (InvalidOperationException)
            {
                // RootViewHint is bogus, i.e. no Dispatcher thread was found
                dialog.ErrorCallback($"Alert failed: RootViewHint {dialog.RootViewHint} can't be mapped to a CoreApplicationView");
            }
        }

        class UIBlock : IUIBlock
        {
            private readonly Action _action;

            public UIBlock(Action action)
            {
                _action = action;
            }

            public void Execute(NativeViewHierarchyManager nativeViewHierarchyManager)
            {
                _action();
            }
        }
    }
}
