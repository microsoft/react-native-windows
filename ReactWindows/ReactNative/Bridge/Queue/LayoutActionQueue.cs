using System;
#if CREATE_LAYOUT_THREAD
using System.Diagnostics;
using System.Runtime.InteropServices;
using Windows.ApplicationModel.Core;
#endif

namespace ReactNative.Bridge.Queue
{
    class LayoutActionQueue : DispatcherActionQueue
    {
#if CREATE_LAYOUT_THREAD
        private const int E_ILLEGAL_METHOD_CALL = -2147483634; // 0x80000000E

        private static readonly Lazy<CoreApplicationView> s_layoutApplicationView = new Lazy<CoreApplicationView>(() =>
        {
            CoreApplicationView newView = null;
            try
            {
                newView = CoreApplication.CreateNewView();
            }
            catch (COMException ex)
            {
                if (ex.HResult == E_ILLEGAL_METHOD_CALL)
                {
                    // Corner case: UWP scenarios that start with no main window.
                    // The OS doesn't allow the creation of new views in this case.
                    // We fall back to main view/dispatcher.
                    Debug.WriteLine("Can't create layout manager view; falling back to main view.");
                    newView = CoreApplication.MainView;
                }
                else
                {
                    throw;
                }
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
