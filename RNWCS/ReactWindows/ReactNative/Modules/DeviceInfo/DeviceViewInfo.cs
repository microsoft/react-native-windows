// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.UIManager;
using Windows.Graphics.Display;
using Windows.UI.ViewManagement;

namespace ReactNative.Modules.DeviceInfo
{
    /// <summary>
    /// Helper class to store info related to application view
    /// </summary>
    class DeviceViewInfo
    {
        public ApplicationView ApplicationView { get; }

        public ReactRootView RootView { get; }

        public DisplayInformation DisplayInformation { get; }

        public int RootViewTag { get; set; }

        public string RootViewId { get; }

        public DisplayMetrics CurrentDisplayMetrics { get; private set; }

        public DeviceViewInfo(ApplicationView applicationView, ReactRootView rootView, DisplayInformation displayInformation, int tag)
        {
            this.ApplicationView = applicationView;
            this.RootView = rootView;
            this.DisplayInformation = displayInformation;
            this.RootViewTag = tag;
            this.RootViewId = rootView.InitialProps?.GetValue("reactxp_rootViewId")?.ToString();
            this.CurrentDisplayMetrics = DisplayMetrics.GetForDeviceView(this);
        }

        public void UpdateDisplayMetrics()
        {
            this.CurrentDisplayMetrics = DispatcherHelpers.CallOnDispatcher(this.RootView.Dispatcher, () => DisplayMetrics.GetForDeviceView(this), true).Result;
        }
    }
}
