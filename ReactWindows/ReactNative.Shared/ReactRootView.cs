// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Touch;
using ReactNative.UIManager;
using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative
{
    /// <summary>
    /// Default root view for applicaitons. Provides the ability to listen for
    /// size changes so that the UI manager can re-layout its elements.
    /// 
    /// It is also responsible for handling touch events passed to any of it's
    /// child views and sending those events to JavaScript via the
    /// <see cref="UIManager.Events.RCTEventEmitter"/> module.
    /// </summary>
    public class ReactRootView : SizeMonitoringCanvas
    {
        private ReactInstanceManager _reactInstanceManager;
        private string _jsModuleName;
        private JObject _initialProps;

        private bool _wasMeasured;
        private bool _attachScheduled;

        /// <summary>
        /// Instantiates the <see cref="ReactRootView"/>.
        /// </summary>
        public ReactRootView()
        {
            TouchHandler = new TouchHandler(this);
        }

        /// <summary>
        /// Gets the JavaScript module name.
        /// </summary>
        internal string JavaScriptModuleName
        {
            get
            {
                return _jsModuleName;
            }
        }

        /// <summary>
        /// The touch handler for the root view.
        /// </summary>
        internal TouchHandler TouchHandler
        {
            get;
        }

        /// <summary>
        /// Get the initialProps
        /// </summary>
        internal JObject InitialProps
        {
            get
            {
                return _initialProps;
            }
        }

        /// <summary>
        /// Schedule rendering of the React component rendered by the 
        /// JavaScript application from the given JavaScript module 
        /// <paramref name="moduleName"/> using the provided
        /// <paramref name="reactInstanceManager"/> to attach to the JavaScript
        /// context of that manager.
        /// </summary>
        /// <remarks>
        /// Has to be called under the dispatcher the view is associated to.
        /// </remarks>
        /// <param name="reactInstanceManager">
        /// The React instance manager.
        /// </param>
        /// <param name="moduleName">The module name.</param>
        public void StartReactApplication(ReactInstanceManager reactInstanceManager, string moduleName)
        {
            StartReactApplication(reactInstanceManager, moduleName, default(JObject));
        }

        /// <summary>
        /// Schedule rendering of the React component rendered by the 
        /// JavaScript application from the given JavaScript module 
        /// <paramref name="moduleName"/> using the provided
        /// <paramref name="reactInstanceManager"/> to attach to the JavaScript context of that manager.
        /// Extra parameter
        /// <paramref name="initialProps"/> can be used to pass initial properties for the react component.
        /// </summary>
        /// <remarks>
        /// Has to be called under the dispatcher associated with the view.
        /// </remarks>
        /// <param name="reactInstanceManager">
        /// The React instance manager.
        /// </param>
        /// <param name="moduleName">The module name.</param>
        /// <param name="initialProps">The initialProps</param>
        ///
        public void StartReactApplication(ReactInstanceManager reactInstanceManager, string moduleName, JObject initialProps)
        {
            // Fire and forget async
            Forget(StartReactApplicationAsync(reactInstanceManager, moduleName, initialProps));
        }

        private async Task StartReactApplicationAsync(ReactInstanceManager reactInstanceManager, string moduleName, JObject initialProps)
        {
            // This is called under the dispatcher associated with the view.
            DispatcherHelpers.AssertOnDispatcher(this);

            if (_reactInstanceManager != null)
            {
                throw new InvalidOperationException("This root view has already been attached to an instance manager.");
            }

            _reactInstanceManager = reactInstanceManager;
            _jsModuleName = moduleName;
            _initialProps = initialProps;

            var getReactContextTaskTask =
                DispatcherHelpers.CallOnDispatcher(async () => await _reactInstanceManager.GetOrCreateReactContextAsync(CancellationToken.None), true);
 
            // We need to wait for the initial `Measure` call, if this view has
            // not yet been measured, we set the `_attachScheduled` flag, which
            // will enable deferred attachment of the root node.
            if (_wasMeasured)
            {
                await _reactInstanceManager.AttachMeasuredRootViewAsync(this);
            }
            else
            {
                _attachScheduled = true;
            }

            await getReactContextTaskTask.Unwrap();
        }

        /// <summary>
        /// Frees resources associated with this root view (fire and forget version)
        /// </summary>
        /// <remarks>
        /// Has to be called under the dispatcher associated with the view.
        /// </remarks>
        public void StopReactApplication()
        {
            Forget(StopReactApplicationAsync());
        }

        /// <summary>
        /// Frees resources associated with this root view.
        /// </summary>
        /// <remarks>
        /// Has to be called under the dispatcher associated with the view.
        /// </remarks>
        public async Task StopReactApplicationAsync()
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            var reactInstanceManager = _reactInstanceManager;
            if (!_attachScheduled && reactInstanceManager != null)
            {
                await reactInstanceManager.DetachRootViewAsync(this);
            }
        }

        /// <summary>
        /// Hooks into the measurement event to potentially attach the React 
        /// root view.
        /// </summary>
        /// <param name="availableSize">The available size.</param>
        /// <returns>The desired size.</returns>
        protected override Size MeasureOverride(Size availableSize)
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            var result = base.MeasureOverride(availableSize);

            // Fire and forget async
            Forget(MeasureOverrideHelperAsync());

            return result;
        }

        internal void CleanupSafe()
        {
            // Inlining allowed
            DispatcherHelpers.RunOnDispatcher(this.Dispatcher, Cleanup, true);
        }

        internal void Cleanup()
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            Children.Clear();
            this.ClearData();
        }

        private async Task MeasureOverrideHelperAsync()
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            _wasMeasured = true;

            var reactInstanceManager = _reactInstanceManager;
            if (_attachScheduled && reactInstanceManager != null)
            {
                _attachScheduled = false;

                await reactInstanceManager.AttachMeasuredRootViewAsync(this);
            }
        }

        private static void Forget(Task task)
        {
            task.ContinueWith(
                t =>
                {
                    Debug.Fail("Exception in fire and forget asynchronous function", t.Exception.ToString());
                },
                TaskContinuationOptions.OnlyOnFaulted);
        }
    }
}
