// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using ReactNative.Common;
#if CREATE_LAYOUT_THREAD
using System.Diagnostics;
using System.Runtime.InteropServices;
using Windows.ApplicationModel.Core;
using ReactNative.Tracing;
#endif

namespace ReactNative.Bridge.Queue
{
    class LayoutActionQueue : DispatcherActionQueue
    {
#if CREATE_LAYOUT_THREAD
        private static readonly Lazy<CoreApplicationView> s_layoutApplicationView = new Lazy<CoreApplicationView>(() =>
        {
            CoreApplicationView newView = null;
            try
            {
                newView = CoreApplication.CreateNewView();
            }
            catch (Exception ex)
            {
                // Corner case: UWP scenarios that starts with no main window.
                // The OS doesn't allow the creation of new views in this case.
                //
                // This is a COMException with HResult == E_ILLEGAL_METHOD_CALL (0x80000000E) when regular managed code,
                // or an InvalidOperationException when using .NET Native.
                // So we don't bother filtering exceptions
                //
                // We fall back to main view/dispatcher permanently, for now.
                // A "fail over" to normal behavior will have to be implemented, eventually
                RnLog.Warn(ReactConstants.RNW, $"Can't create layout manager view; falling back to main view.");
                newView = CoreApplication.MainView;
            }
            return newView;
        });
           
#endif
        public LayoutActionQueue(Action<Exception> onError)
#if CREATE_LAYOUT_THREAD
            : base(onError, s_layoutApplicationView.Value.Dispatcher)
#else
            : base(onError, DispatcherHelpers.MainDispatcher)
#endif
        {
        }
    }
}
