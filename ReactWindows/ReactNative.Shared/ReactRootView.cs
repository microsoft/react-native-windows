using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Touch;
using System;
using System.Threading;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Foundation;
#else
using System.Windows;
#endif

namespace ReactNative
{
    /// <summary>
    /// Default root view for applications. Provides the ability to listen for
    /// size changes so that the UI manager can re-layout its elements.
    /// 
    /// It is also responsible for handling touch events passed to any of it's
    /// child views and sending those events to JavaScript via the
    /// <see cref="UIManager.Events.RCTEventEmitter"/> module.
    /// </summary>
    public class ReactRootView : ReactRootViewBase
    {
        enum AttachState
        {
            Detached,
            Scheduled,
            Attached
        }

        private ReactInstanceManager _reactInstanceManager;
        private string _jsModuleName;
        private JObject _initialProps;

        private bool _wasMeasured;
        private AttachState _attachState = AttachState.Detached;

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
        public override string JavaScriptModuleName
        {
            get
            {
                return _jsModuleName;
            }
            set
            {
                throw new NotSupportedException();
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
        /// The props value passed to the React component
        /// </summary>
        public override JObject InitialProps
        {
            get
            {
                return _initialProps;
            }
            set
            {
                if (value != _initialProps)
                {
                    _initialProps = value;
                    if (_attachState == AttachState.Attached)
                    {
                        _reactInstanceManager.UpdatePropsForRootView(this);
                    }
                }
            }
        }

        /// <summary>
        /// Schedule rendering of the React component rendered by the 
        /// JavaScript application from the given JavaScript module 
        /// <paramref name="moduleName"/> using the provided
        /// <paramref name="reactInstanceManager"/> to attach to the JavaScript
        /// context of that manager.
        /// </summary>
        /// <param name="reactInstanceManager">
        /// The React instance manager.
        /// </param>
        /// <param name="moduleName">The module name.</param>
        public void StartReactApplication(ReactInstanceManager reactInstanceManager, string moduleName)
        {
            StartReactApplication(reactInstanceManager, moduleName, InitialProps);
        }

        /// <summary>
        /// Schedule rendering of the React component rendered by the 
        /// JavaScript application from the given JavaScript module 
        /// <paramref name="moduleName"/> using the provided
        /// <paramref name="reactInstanceManager"/> to attach to the JavaScript context of that manager.
        /// Extra parameter
        /// <paramref name="initialProps"/> can be used to pass initial properties for the react component.
        /// </summary>
        /// <param name="reactInstanceManager">
        /// The React instance manager.
        /// </param>
        /// <param name="moduleName">The module name.</param>
        /// <param name="initialProps">The initialProps</param>
        public async void StartReactApplication(ReactInstanceManager reactInstanceManager, string moduleName, JObject initialProps)
        {
            DispatcherHelpers.AssertOnDispatcher();

            if (_reactInstanceManager != null)
            {
                throw new InvalidOperationException("This root view has already been attached to an instance manager.");
            }

            _reactInstanceManager = reactInstanceManager;
            _jsModuleName = moduleName;
            _initialProps = initialProps;

            var getReactContextTask = default(Task);
            if (!_reactInstanceManager.HasStartedCreatingInitialContext)
            {
                getReactContextTask = _reactInstanceManager.GetOrCreateReactContextAsync(CancellationToken.None);
            }

            // We need to wait for the initial `Measure` call, if this view has
            // not yet been measured, we set the `_attachScheduled` flag, which
            // will enable deferred attachment of the root node.
            if (_wasMeasured)
            {
                _reactInstanceManager.AttachMeasuredRootView(this);
                _attachState = AttachState.Attached;
            }
            else
            {
                _attachState = AttachState.Scheduled;
            }

            if (getReactContextTask != null)
            {
                await getReactContextTask.ConfigureAwait(false);
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
            DispatcherHelpers.AssertOnDispatcher();

            var result = base.MeasureOverride(availableSize);

            _wasMeasured = true;

            var reactInstanceManager = _reactInstanceManager;
            if (_attachState == AttachState.Scheduled && reactInstanceManager != null)
            {
                _attachState = AttachState.Attached;
                reactInstanceManager.AttachMeasuredRootView(this);
            }

            return result;
        }
    }
}
