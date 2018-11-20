// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Touch;
using ReactNative.Tracing;
using ReactNative.UIManager;
using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
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
            get; private set;
        }

        /// <summary>
        /// The initial props.
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
        /// JavaScript application from the given JavaScript module. 
        /// <paramref name="moduleName"/> using the provided
        /// <paramref name="reactInstanceManager"/> to attach to the JavaScript context of that manager.
        /// Extra parameter
        /// <paramref name="initialProps"/> can be used to pass initial props for the React component.
        /// </summary>
        /// <remarks>
        /// Has to be called under the dispatcher associated with the view.
        /// </remarks>
        /// <param name="reactInstanceManager">
        /// The React instance manager.
        /// </param>
        /// <param name="moduleName">The module name.</param>
        /// <param name="initialProps">The initial props.</param>
        public void StartReactApplication(ReactInstanceManager reactInstanceManager, string moduleName, JObject initialProps)
        {
            // Fire and forget async
            Forget(StartReactApplicationAsync(reactInstanceManager, moduleName, initialProps));
        }

        private async Task StartReactApplicationAsync(ReactInstanceManager reactInstanceManager, string moduleName, JObject initialProps)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactRootView: StartReactApplicationAsync ({moduleName}) - entry");
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

            await getReactContextTaskTask.Unwrap();

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

            RnLog.Info(ReactConstants.RNW, $"ReactRootView: StartReactApplicationAsync ({moduleName}) - done ({(_attachScheduled ? "with scheduled work" : "completely")})");
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
        /// <returns>Awaitable task.</returns>
        public async Task StopReactApplicationAsync()
        {
            RnLog.Info(ReactConstants.RNW, $"ReactRootView: StopReactApplicationAsync ({JavaScriptModuleName}) - entry");
            DispatcherHelpers.AssertOnDispatcher(this);

            var reactInstanceManager = _reactInstanceManager;
            var attachScheduled = _attachScheduled;
            _attachScheduled = false;
            if (!attachScheduled && reactInstanceManager != null)
            {
                await reactInstanceManager.DetachRootViewAsync(this);
            }

            RnLog.Info(ReactConstants.RNW, $"ReactRootView: StopReactApplicationAsync ({JavaScriptModuleName}) - done");
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

        internal void StartTouchHandling()
        {
            if (TouchHandler == null)
            {
                TouchHandler = new TouchHandler(this);
            }
        }

        internal void StopTouchHandling()
        {
            if (TouchHandler != null)
            {
                TouchHandler.Dispose();
                TouchHandler = null;
            }
        }

#if WINDOWS_UWP
        /// <summary>
        /// Override ensuring the creation of an AutomationPeer for ReactRootView 
        /// root view.
        /// </summary>
        /// <returns>The new AutomationPeer.</returns>
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            //
            // Older Windows OS'es (RS1, for ex.) don't create automation peers for normal Canvas elements.
            // This is not an issue in general, but for ReactRootView in particular is, since it is the root of accessibility traversal
            // done in AccessibilityHelper and so it needs a peer.
            var peer = base.OnCreateAutomationPeer();
            if (peer == null)
            {
                // Fallback to creating peer if base didn't provide one.
                peer = new FrameworkElementAutomationPeer(this);
            }
            return peer;
        }
#endif

        private async Task MeasureOverrideHelperAsync()
        {
            DispatcherHelpers.AssertOnDispatcher(this);

            _wasMeasured = true;

            var reactInstanceManager = _reactInstanceManager;
            var attachScheduled = _attachScheduled;
            _attachScheduled = false;
            if (attachScheduled && reactInstanceManager != null)
            {
                await reactInstanceManager.AttachMeasuredRootViewAsync(this);
            }
        }

        private static void Forget(Task task)
        {
            task.ContinueWith(
                t =>
                {
                    RnLog.Fatal(ReactConstants.RNW, t.Exception, $"Exception in fire and forget asynchronous function");
                },
                TaskContinuationOptions.OnlyOnFaulted);
        }
    }
}
