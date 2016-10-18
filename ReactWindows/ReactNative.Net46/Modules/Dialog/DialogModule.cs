using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using System.Collections.Generic;
using System.Windows;

namespace ReactNative.Modules.Dialog
{
    class DialogModule : ReactContextNativeModuleBase
    {
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

        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return new Dictionary<string, object>
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