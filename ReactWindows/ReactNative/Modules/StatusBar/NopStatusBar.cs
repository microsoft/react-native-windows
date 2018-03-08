// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;

namespace ReactNative.Modules.StatusBar
{
    class NopStatusBar : IStatusBar
    {
        public Color? BackgroundColor
        {
            get;
            set;
        }

        public double BackgroundOpacity
        {
            get;
            set;
        }

        public IAsyncAction HideAsync()
        {
            return Task.CompletedTask.AsAsyncAction();
        }

        public IAsyncAction ShowAsync()
        {
            return Task.CompletedTask.AsAsyncAction();
        }
    }
}
