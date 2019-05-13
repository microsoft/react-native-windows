// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Foundation;
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    class ReactDefaultCompoundView : IReactCompoundView
    {
        public int GetReactTagAtPoint(UIElement reactView, Point point)
        {
            return reactView.GetTag();
        }
    }
}
