// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading;
using System.Windows.Threading;

namespace ReactNative.Bridge.Queue
{
    class LayoutActionQueue : DispatcherActionQueue
    {
        private readonly DispatcherManager _dispatcherManager;

        public LayoutActionQueue(Action<Exception> onError)
            : this(onError, new DispatcherManager())
        {
        }

        private LayoutActionQueue(Action<Exception> onError, DispatcherManager dispatcherManager)
            : base(onError, dispatcherManager.Instance)
        {
            _dispatcherManager = dispatcherManager;
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
            _dispatcherManager.Dispose();
        }

        class DispatcherManager : IDisposable
        {
            private readonly Thread _threadInstance;

            public DispatcherManager()
            {
                var ready = new ManualResetEvent(false);
                _threadInstance = new Thread(new ThreadStart(() => {
                    Instance = Dispatcher.CurrentDispatcher;
                    ready.Set();
                    Dispatcher.Run();
                }));

                _threadInstance.SetApartmentState(ApartmentState.STA);
                _threadInstance.Start();
                ready.WaitOne();
            }

            public Dispatcher Instance
            {
                get;
                private set;
            }

            public void Dispose()
            {
                Instance.InvokeShutdown();
                _threadInstance.Join();
            }
        }
    }
}
