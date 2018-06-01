// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Graphics.Display;
using Windows.UI.ViewManagement;
using ReactNative.Bridge;
using ReactNative.UIManager;

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

        public DisplayMetrics CurrentDisplayMetrics { get; private set; }

        public DeviceViewInfo(ApplicationView applicationView, ReactRootView rootView, DisplayInformation displayInformation, int tag)
        {
            this.ApplicationView = applicationView;
            this.RootView = rootView;
            this.DisplayInformation = displayInformation;
            this.RootViewTag = tag;

            this.CurrentDisplayMetrics = DisplayMetrics.GetForDeviceView(this);
        }

        public void UpdateDisplayMetrics()
        {
            this.CurrentDisplayMetrics = DispatcherHelpers.CallOnDispatcher(this.RootView.Dispatcher, () => DisplayMetrics.GetForDeviceView(this), true).Result;
        }
    }
}
