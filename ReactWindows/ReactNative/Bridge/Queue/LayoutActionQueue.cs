using System;
#if CREATE_LAYOUT_THREAD
using Windows.ApplicationModel.Core;
#endif

namespace ReactNative.Bridge.Queue
{
    class LayoutActionQueue : DispatcherActionQueue
    {
#if CREATE_LAYOUT_THREAD
        private static readonly CoreApplicationView s_layoutApplicationView = CoreApplication.CreateNewView();

#endif
        public LayoutActionQueue(Action<Exception> onError)
#if CREATE_LAYOUT_THREAD
            : base(onError, s_layoutApplicationView.Dispatcher)
#else
            : base(onError, DispatcherHelpers.MainDispatcher)
#endif
        {
        }
    }
}
