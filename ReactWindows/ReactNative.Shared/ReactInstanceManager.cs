// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.Common;
using ReactNative.DevSupport;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Reactive.Disposables;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative
{
    /// <summary>
    /// This interface manages instances of <see cref="IReactInstance" />.
    /// It exposes a way to configure React instances using
    /// <see cref="IReactPackage"/> and keeps track of the lifecycle of that
    /// instance. It also sets up a connection between the instance and the
    /// developer support functionality of the framework.
    ///
    /// An instance of this manager is required to start the JavaScript
    /// application in <see cref="ReactRootView"/>
    /// (<see cref="ReactRootView.StartReactApplication(ReactInstanceManager, string)"/>).
    ///
    /// The lifecycle of the instance of <see cref="ReactInstanceManager"/>
    /// should be bound to the application that owns the
    /// <see cref="ReactRootView"/> that is used to render the React
    /// application using this instance manager. It is required to pass
    /// lifecycle events to the instance manager (i.e., <see cref="OnSuspend"/>,
    /// <see cref="IAsyncDisposable.DisposeAsync"/>, and <see cref="OnResume(Action)"/>).
    /// </summary>
    public class ReactInstanceManager
    {
        // Awaitable lock syncronizing the entire initialization of ReactContext
        private readonly AsyncLock _lock = new AsyncLock();

        // Threading
        // - Most of public APIs have to be called on main dispatcher thread, with the exception of
        // AttachMeasuredRootViewAsync and DetachRootViewAsync (called under the dispatcher corresponding to the
        // rootView parameter)
        // - The internal context initialization is heavily asynchronous, so it uses an AsyncLock to prevent concurrent initialization
        // in a non-blocking manner
        // - The "scope" of this AsyncLock extends to the DevSupportManager as well, all the private methods involved in creating a React context
        // in any way are called with the AsyncLock held.
        //
        private ReactContext _currentReactContext;
        private readonly List<ReactRootView> _attachedRootViews = new List<ReactRootView>();

        private readonly SerialDisposable _currentInitializationToken = new SerialDisposable();

        private readonly string _jsBundleFile;
        private readonly string _jsMainModuleName;
        private readonly IReadOnlyList<IReactPackage> _packages;
        private readonly IDevSupportManager _devSupportManager;
        private readonly bool _useDeveloperSupport;
        private readonly UIImplementationProvider _uiImplementationProvider;
        private readonly Func<IJavaScriptExecutor> _javaScriptExecutorFactory;
        private readonly Action<Exception> _nativeModuleCallExceptionHandler;
        private readonly bool _lazyViewManagersEnabled;

        private LifecycleStateMachine _lifecycleStateMachine;
        private CancellationDisposable _suspendCancellation;
        private string _sourceUrl;
        private Action _defaultBackButtonHandler;

        internal ReactInstanceManager(
            string jsBundleFile,
            string jsMainModuleName,
            IReadOnlyList<IReactPackage> packages,
            bool useDeveloperSupport,
            LifecycleState initialLifecycleState,
            UIImplementationProvider uiImplementationProvider,
            Func<IJavaScriptExecutor> javaScriptExecutorFactory,
            Action<Exception> nativeModuleCallExceptionHandler,
            bool lazyViewManagersEnabled)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: constructor");

            if (packages == null)
                throw new ArgumentNullException(nameof(packages));
            if (uiImplementationProvider == null)
                throw new ArgumentNullException(nameof(uiImplementationProvider));
            if (javaScriptExecutorFactory == null)
                throw new ArgumentNullException(nameof(javaScriptExecutorFactory));

            _jsBundleFile = jsBundleFile;
            _jsMainModuleName = jsMainModuleName;
            _packages = packages;

            _useDeveloperSupport = useDeveloperSupport;
            _devSupportManager = _useDeveloperSupport
                ? (IDevSupportManager)new DevSupportManager(
                    new ReactInstanceDevCommandsHandler(this),
                    _jsBundleFile == null,
                    _jsMainModuleName)
                : new DisabledDevSupportManager();

            _lifecycleStateMachine = new LifecycleStateMachine(initialLifecycleState);
            _uiImplementationProvider = uiImplementationProvider;
            _javaScriptExecutorFactory = javaScriptExecutorFactory;
            _nativeModuleCallExceptionHandler = nativeModuleCallExceptionHandler;
            _lazyViewManagersEnabled = lazyViewManagersEnabled;
        }
        
        /// <summary>
        /// The developer support manager for the instance.
        /// </summary>
        public IDevSupportManager DevSupportManager
        {
            get
            {
                return _devSupportManager;
            }
        }

        /// <summary>
        /// The URL where the last bundle was loaded from.
        /// </summary>
        public string SourceUrl
        {
            get
            {
                return _sourceUrl;
            }
        }

        /// <summary>
        /// Gets the current React context instance.
        /// </summary>
        public ReactContext CurrentReactContext
        {
            get
            {
                return _currentReactContext;
            }
        }

        /// <summary>
        /// Trigger the React context initialization asynchronously in a
        /// background task. This enables applications to pre-load the
        /// application JavaScript, and execute global core code before the
        /// <see cref="ReactRootView"/> is available and measure. This should
        /// only be called the first time the application is set up, which is
        /// enforced to keep developers from accidentally creating their
        /// applications multiple times.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>A task to await the result.</returns>
        public async Task<ReactContext> CreateReactContextAsync(CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: CreateReactContextAsync - entry");
            DispatcherHelpers.AssertOnDispatcher();
            using (await _lock.LockAsync())
            {
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: CreateReactContextAsync - execute");
                if (_currentReactContext != null)
                {
                    throw new InvalidOperationException(
                            "React context creation should only be called when creating the React " +
                            "application for the first time. When reloading JavaScript, e.g., from " +
                            "a new file, explicitly, use the re-create method.");
                }

                await CreateReactContextCoreAsync(token);

                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: CreateReactContextAsync - returning {(_currentReactContext == null ? "null" : "valid")} context");
                return _currentReactContext;
            }
        }

        /// <summary>
        /// Awaits the currently initializing React context.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>
        /// A task to await the React context.
        /// </returns>
        public async Task<ReactContext> GetReactContextAsync(CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetReactContextAsync - entry");
            DispatcherHelpers.AssertOnDispatcher();
            using (await _lock.LockAsync())
            {
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetReactContextAsync - execute");
                if (_currentReactContext == null)
                {
                    throw new InvalidOperationException(
                        "Use the create method to start initializing the React context.");
                }

                // By this point context has already been created due to the serialized aspect of context initialization.
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetReactContextAsync - returning existing {(_currentReactContext == null ? "null" : "valid")} context");
                return _currentReactContext;
            }
        }

        /// <summary>
        /// Awaits the currently initializing React context, or returns null if context fails to initialize.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>
        /// A task to await the React context.
        /// </returns>
        public async Task<ReactContext> TryGetReactContextAsync(CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: TryGetReactContextAsync - entry");
            DispatcherHelpers.AssertOnDispatcher();
            using (await _lock.LockAsync())
            {
                // By this point context has already been created due to the serialized aspect of context initialization.
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: TryGetReactContextAsync - execute/returning existing {(_currentReactContext == null ? "null" : "valid")} context");
                return _currentReactContext;
            }
        }

        /// <summary>
        /// Awaits the currently initializing React context, or creates a new one.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>
        /// A task to await the React context.
        /// </returns>
        public async Task<ReactContext> GetOrCreateReactContextAsync(CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetOrCreateReactContextAsync - entry");
            DispatcherHelpers.AssertOnDispatcher();
            using (await _lock.LockAsync())
            {
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetOrCreateReactContextAsync - execute");
                if (_currentReactContext != null)
                {
                    // By this point context has already been created due to the serialized aspect of context initialization.
                    RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetOrCreateReactContextAsync - returning existing {(_currentReactContext == null ? "null" : "valid")} context");
                    return _currentReactContext;
                }
                else
                {
                    await CreateReactContextCoreAsync(token);

                    RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: GetOrCreateReactContextAsync - returning {(_currentReactContext == null ? "null" : "valid")} context");
                    return _currentReactContext;
                }
            }
        }

        /// <summary>
        /// Recreate the React application and context. This should be called
        /// if configuration has changed or the developer has requested the
        /// application to be reloaded.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>A task to await the result.</returns>
        public async Task<ReactContext> RecreateReactContextAsync(CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: RecreateReactContextAsync - entry");
            DispatcherHelpers.AssertOnDispatcher();
            using (await _lock.LockAsync())
            {
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: RecreateReactContextAsync - execute");
                if (_currentReactContext == null)
                {
                    throw new InvalidOperationException(
                        "React context re-creation should only be called after the initial " +
                        "create context background call.");
                }

                await CreateReactContextCoreAsync(token);

                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: RecreateReactContextAsync - returning {(_currentReactContext == null ? "null" : "valid")} context");
                return _currentReactContext;
            }
        }

        /// <summary>
        /// Method that gives JavaScript the opportunity to consume the back
        /// button event. If JavaScript does not consume the event, the
        /// default back press action will be invoked at the end of the
        /// roundtrip to JavaScript.
        /// </summary>
        public void OnBackPressed()
        {
            DispatcherHelpers.AssertOnDispatcher();
            var reactContext = _currentReactContext;
            if (reactContext == null)
            {
                RnLog.Warn(ReactConstants.RNW, $"ReactInstanceManager: OnBackPressed: Instance detached from instance manager.");
                InvokeDefaultOnBackPressed();
            }
            else
            {
                reactContext.GetNativeModule<DeviceEventManagerModule>().EmitHardwareBackPressed();
            }
        }

        /// <summary>
        /// Called when the application is suspended.
        /// </summary>
        public void OnSuspend()
        {
            DispatcherHelpers.AssertOnDispatcher();

            _defaultBackButtonHandler = null;
            _suspendCancellation?.Dispose();

            if (_useDeveloperSupport)
            {
                _devSupportManager.IsEnabled = false;
            }

            _lifecycleStateMachine.OnSuspend();
        }

        /// <summary>
        /// Called when the host entered background mode.
        /// </summary>
        public void OnEnteredBackground()
        {
            DispatcherHelpers.AssertOnDispatcher();
            _lifecycleStateMachine.OnEnteredBackground();
        }

        /// <summary>
        /// Called when the host is leaving background mode.
        /// </summary>
        public void OnLeavingBackground()
        {
            DispatcherHelpers.AssertOnDispatcher();
            _lifecycleStateMachine.OnLeavingBackground();
        }

        /// <summary>
        /// Used when the application resumes to reset the back button handling
        /// in JavaScript.
        /// </summary>
        /// <param name="onBackPressed">
        /// The action to take when back is pressed.
        /// </param>
        public void OnResume(Action onBackPressed)
        {
            DispatcherHelpers.Initialize();
            DispatcherHelpers.AssertOnDispatcher();
            ReactChoreographer.Initialize();

            _defaultBackButtonHandler = onBackPressed;
            _suspendCancellation = new CancellationDisposable();

            if (_useDeveloperSupport)
            {
                _devSupportManager.IsEnabled = true;
            }

            _lifecycleStateMachine.OnResume();
        }

        /// <summary>
        /// Destroy the <see cref="ReactInstanceManager"/>.
        /// </summary>
        public async Task DisposeAsync()
        {
            DispatcherHelpers.AssertOnDispatcher();
            using (await _lock.LockAsync())
            {
                // TODO: memory pressure hooks
                if (_useDeveloperSupport)
                {
                    _devSupportManager.IsEnabled = false;
                }

                _lifecycleStateMachine.OnDestroy();
                _lifecycleStateMachine.SetContext(null);

                var currentReactContext = _currentReactContext;
                if (currentReactContext != null)
                {
                    await currentReactContext.DisposeAsync();
                    _currentReactContext = null;
                }

                ReactChoreographer.Dispose();
                DispatcherHelpers.Reset();
            }
        }

        /// <summary>
        /// Attach given <paramref name="rootView"/> to a React instance
        /// manager and start the JavaScript application using the JavaScript
        /// module provided by the <see cref="ReactRootView.JavaScriptModuleName"/>. If
        /// the React context is currently being (re-)created, or if the react
        /// context has not been created yet, the JavaScript application
        /// associated with the provided root view will be started
        /// asynchronously. This view will then be tracked by this manager and
        /// in case of React instance restart, it will be re-attached.
        /// WARNING! Has to be called by the thread associated with the view.
        /// </summary>
        /// <param name="rootView">The root view.</param>
        public async Task AttachMeasuredRootViewAsync(ReactRootView rootView)
        {
            if (rootView == null)
                throw new ArgumentNullException(nameof(rootView));

            DispatcherHelpers.AssertOnDispatcher(rootView);
            rootView.Children.Clear();
            ViewExtensions.ClearData(rootView);

            await DispatcherHelpers.CallOnDispatcher(async () =>
            {
                _attachedRootViews.Add(rootView);

                // If the React context is being created in the background, the
                // JavaScript application will be started automatically when
                // creation completes, as root view is part of the attached root
                // view list.
                var currentReactContext = _currentReactContext;
                if (currentReactContext != null)
                {
                    await AttachMeasuredRootViewToInstanceAsync(rootView, currentReactContext.ReactInstance);
                }

                return true;
           }, true).Unwrap(); // inlining allowed
        }

        /// <summary>
        /// Detach given <paramref name="rootView"/> from the current react
        /// instance. This method is idempotent and can be called multiple
        /// times on the same <see cref="ReactRootView"/> instance.
        /// WARNING! Has to be called by the thread assocviated with the view.
        /// </summary>
        /// <param name="rootView">The root view.</param>
        public async Task DetachRootViewAsync(ReactRootView rootView)
        {
            if (rootView == null)
                throw new ArgumentNullException(nameof(rootView));

            DispatcherHelpers.AssertOnDispatcher(rootView);

            await DispatcherHelpers.CallOnDispatcher(() =>
            {
                if (_attachedRootViews.Remove(rootView))
                {
                    var currentReactContext = _currentReactContext;
                    if (currentReactContext != null && currentReactContext.HasActiveReactInstance)
                    {
                        return DetachViewFromInstanceAsync(rootView, currentReactContext.ReactInstance);
                    }
                }

                return Task.CompletedTask;
            }, true /* inlining allowed */).Unwrap(); // await inner task
        }

        /// <summary>
        /// Uses the configured <see cref="IReactPackage"/> instances to create
        /// all <see cref="IViewManager"/> instances.
        /// </summary>
        /// <param name="reactContext">
        /// The application context.
        /// </param>
        /// <returns>The list of view managers.</returns>
        public IReadOnlyList<IViewManager> CreateAllViewManagers(ReactContext reactContext)
        {
            DispatcherHelpers.AssertOnDispatcher();

            if (reactContext == null)
                throw new ArgumentNullException(nameof(reactContext));

            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "createAllViewManagers").Start())
            {
                var allViewManagers = new List<IViewManager>();
                foreach (var package in _packages)
                {
                    allViewManagers.AddRange(
                        package.CreateViewManagers(reactContext));
                }

                return allViewManagers;
            }
        }

        private Task<ReactContext> CreateReactContextCoreAsync(CancellationToken token)
        {
            DispatcherHelpers.AssertOnDispatcher();

            if (_useDeveloperSupport && _jsBundleFile == null)
            {
                return CreateReactContextFromDevManagerAsync(token);
            }
            else
            {
                return CreateReactContextFromBundleAsync(token);
            }
        }

        private Task<ReactContext> CreateReactContextFromDevManagerAsync(CancellationToken token)
        {
            if (_devSupportManager.HasUpToDateBundleInCache())
            {
                return CreateReactContextFromCachedPackagerBundleAsync(token);
            }
            else
            {
                return _devSupportManager.CreateReactContextFromPackagerAsync(token);
            }
        }

        private Task<ReactContext> CreateReactContextFromBundleAsync(CancellationToken token)
        {
            return CreateReactContextAsync(
                    _javaScriptExecutorFactory,
                    JavaScriptBundleLoader.CreateFileLoader(_jsBundleFile),
                    token);
        }

        private Task<ReactContext> CreateReactContextFromCachedPackagerBundleAsync(CancellationToken token)
        {
            var bundleLoader = JavaScriptBundleLoader.CreateCachedBundleFromNetworkLoader(
                _devSupportManager.SourceUrl,
                _devSupportManager.DownloadedJavaScriptBundleFile);
            return CreateReactContextAsync(_javaScriptExecutorFactory, bundleLoader, token);
       }

        private Task<ReactContext> CreateReactContextWithRemoteDebuggerAsync(
            Func<IJavaScriptExecutor> javaScriptExecutorFactory,
            CancellationToken token)
        {
            var bundleLoader = JavaScriptBundleLoader.CreateRemoteDebuggerLoader(
                _devSupportManager.JavaScriptBundleUrlForRemoteDebugging,
                _devSupportManager.SourceUrl);
            return CreateReactContextAsync(javaScriptExecutorFactory, bundleLoader, token);
        }

        private async Task<ReactContext> CreateReactContextAsync(
            Func<IJavaScriptExecutor> jsExecutorFactory,
            JavaScriptBundleLoader jsBundleLoader,
            CancellationToken token)
        {
            var cancellationDisposable = new CancellationDisposable();
            _currentInitializationToken.Disposable = cancellationDisposable;
            using (token.Register(cancellationDisposable.Dispose))
            using (_suspendCancellation?.Token.Register(cancellationDisposable.Dispose))
            {
                try
                {
                    cancellationDisposable.Token.ThrowIfCancellationRequested();
                    return await InitializeReactContextAsync(
                        jsExecutorFactory,
                        jsBundleLoader,
                        cancellationDisposable.Token);
                }
                catch (OperationCanceledException)
                when (cancellationDisposable.Token.IsCancellationRequested)
                {
                    token.ThrowIfCancellationRequested();
                    return null;
                }
            }
        }

        private async Task<ReactContext> InitializeReactContextAsync(
            Func<IJavaScriptExecutor> jsExecutorFactory,
            JavaScriptBundleLoader jsBundleLoader,
            CancellationToken token)
        {
            var currentReactContext = _currentReactContext;
            if (currentReactContext != null)
            {
                await TearDownReactContextAsync(currentReactContext, token);
                _currentReactContext = null;
            }

            try
            {
                var reactContext = await CreateReactContextCoreAsync(jsExecutorFactory, jsBundleLoader, token);
                await SetupReactContextAsync(reactContext);
                return reactContext;
            }
            catch (OperationCanceledException)
            when (token.IsCancellationRequested)
            {
                RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Creating React context has been canceled");
                throw;
            }
            catch (Exception ex)
            {
                RnLog.Error(ReactConstants.RNW, ex, $"ReactInstanceManager: Exception when creating React context: {ex.Message}");
                _devSupportManager.HandleException(ex);
            }

            return null;
        }

        private async Task SetupReactContextAsync(ReactContext reactContext)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Setting up React context - entry");
            DispatcherHelpers.AssertOnDispatcher();
            if (_currentReactContext != null)
            {
                throw new InvalidOperationException(
                    "React context has already been setup and has not been destroyed.");
            }

            _currentReactContext = reactContext;
            var reactInstance = reactContext.ReactInstance;
            _devSupportManager.OnNewReactContextCreated(reactContext);
            // TODO: set up memory pressure hooks
            _lifecycleStateMachine.SetContext(reactContext);

            foreach (var rootView in _attachedRootViews)
            {
                await AttachMeasuredRootViewToInstanceAsync(rootView, reactInstance);
            }
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Setting up React context - done");
        }

        private void InvokeDefaultOnBackPressed()
        {
            DispatcherHelpers.AssertOnDispatcher();
            _defaultBackButtonHandler?.Invoke();
        }

        private async Task AttachMeasuredRootViewToInstanceAsync(
            ReactRootView rootView,
            IReactInstance reactInstance)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: AttachMeasuredRootViewToInstanceAsync ({rootView.JavaScriptModuleName}) - entry");

            DispatcherHelpers.AssertOnDispatcher();
            var rootTag = await reactInstance.GetNativeModule<UIManagerModule>()
                .AddMeasuredRootViewAsync(rootView);

            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: AttachMeasuredRootViewToInstanceAsync ({rootView.JavaScriptModuleName}) - added to UIManager (tag: {rootTag}), starting React app");

            var jsAppModuleName = rootView.JavaScriptModuleName;
            var appParameters = new Dictionary<string, object>
            {
                { "rootTag", rootTag },
                { "initialProps", rootView.InitialProps }
            };

            reactInstance.GetJavaScriptModule<AppRegistry>().runApplication(jsAppModuleName, appParameters);
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: AttachMeasuredRootViewToInstanceAsync ({rootView.JavaScriptModuleName}) - done");
        }

        private async Task DetachViewFromInstanceAsync(
            ReactRootView rootView,
            IReactInstance reactInstance)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: DetachViewFromInstanceAsync ({rootView.JavaScriptModuleName}) - entry");
            DispatcherHelpers.AssertOnDispatcher();

            // Detaches ReactRootView from instance manager root view list and size change monitoring.
            // This has to complete before unmounting the application.
            // Returns a task to await the completion of the `removeRootView` UIManager call (an effect of
            // unmounting the application) and the release of all dispatcher affined UI objects.
            var rootViewRemovedTask = await reactInstance.GetNativeModule<UIManagerModule>()
                .DetachRootViewAsync(rootView);

            reactInstance.GetJavaScriptModule<AppRegistry>().unmountApplicationComponentAtRootTag(rootView.GetTag());

            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: DetachViewFromInstanceAsync ({rootView.JavaScriptModuleName}) - waiting for removeRootView({rootView.GetTag()})");

            await rootViewRemovedTask;

            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: DetachViewFromInstanceAsync ({rootView.JavaScriptModuleName}) - done");
        }

        private async Task TearDownReactContextAsync(ReactContext reactContext, CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Tearing down React context - entry");
            token.ThrowIfCancellationRequested();

            DispatcherHelpers.AssertOnDispatcher();

            _lifecycleStateMachine.SetContext(null);

            // Existing root views should be silenced before tearing down the context.
            // Most of the work is done by the tearing down of the context itself, yet the native root views continue to exist,
            // so things like "size changes" or touch handling should be stopped immediately.
            foreach (var rootView in _attachedRootViews)
            {
                // Inlining allowed
                DispatcherHelpers.RunOnDispatcher(rootView.Dispatcher, () =>
                {
                    rootView.RemoveSizeChanged();

                    rootView.StopTouchHandling();
                }, true);
            }

            await reactContext.DisposeAsync();
            _devSupportManager.OnReactContextDestroyed(reactContext);

            // TODO: add memory pressure hooks
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Tearing down React context - done");
        }

        private async Task<ReactContext> CreateReactContextCoreAsync(
            Func<IJavaScriptExecutor> jsExecutorFactory,
            JavaScriptBundleLoader jsBundleLoader,
            CancellationToken token)
        {
            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Creating React context - entry");

            _sourceUrl = jsBundleLoader.SourceUrl;

            var reactContext = new ReactContext();
            if (_useDeveloperSupport)
            {
                reactContext.NativeModuleCallExceptionHandler =
                    _nativeModuleCallExceptionHandler ?? _devSupportManager.HandleException;
            }

            var nativeRegistryBuilder = new NativeModuleRegistry.Builder(reactContext);
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "createAndProcessCoreModulesPackage").Start())
            {
                var coreModulesPackage = new CoreModulesPackage(
                    this,
                    InvokeDefaultOnBackPressed,
                    _uiImplementationProvider,
                    _lazyViewManagersEnabled);

                var coreModulesPackageWrapper = new CoreModulesPackageWrapper(coreModulesPackage);

                ProcessPackage(coreModulesPackageWrapper, reactContext, nativeRegistryBuilder);
            }

            foreach (var reactPackage in _packages)
            {
                using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "createAndProcessCustomReactPackage").Start())
                {
                    ProcessPackage(reactPackage, reactContext, nativeRegistryBuilder);
                }
            }

            var nativeModuleRegistry = default(NativeModuleRegistry);
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "buildNativeModuleRegistry").Start())
            {
                nativeModuleRegistry = nativeRegistryBuilder.Build();
            }

            var queueConfiguration = ReactQueueConfigurationFactory.Default.Create(reactContext.HandleException);
            var reactInstanceBuilder = new ReactInstance.Builder
            {
                QueueConfiguration = queueConfiguration,
                JavaScriptExecutorFactory = jsExecutorFactory,
                Registry = nativeModuleRegistry,
                BundleLoader = jsBundleLoader,
            };

            var reactInstance = default(ReactInstance);
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "createReactInstance").Start())
            {
                reactInstance = reactInstanceBuilder.Build();
            }

            // TODO: add bridge idle debug listener

            reactContext.InitializeWithInstance(reactInstance);

            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "RunJavaScriptBundle").Start())
            {
                await reactInstance.InitializeBridgeAsync(token);
            }

            await reactInstance.InitializeAsync().ConfigureAwait(false);

            RnLog.Info(ReactConstants.RNW, $"ReactInstanceManager: Creating React context - done");
            return reactContext;
        }

        private void ProcessPackage(
            IReactPackage reactPackage,
            ReactContext reactContext,
            NativeModuleRegistry.Builder nativeRegistryBuilder)
        {
            foreach (var nativeModule in reactPackage.CreateNativeModules(reactContext))
            {
                nativeRegistryBuilder.Add(nativeModule);
            }
        }

        class ReactInstanceDevCommandsHandler : IReactInstanceDevCommandsHandler
        {
            private readonly ReactInstanceManager _parent;

            public ReactInstanceDevCommandsHandler(ReactInstanceManager parent)
            {
                _parent = parent;
            }

            public Task<IDisposable> LockAsync()
            {
                return _parent._lock.LockAsync();
            }

            public Task<ReactContext> CreateReactContextFromBundleAsync(CancellationToken token)
            {
                return _parent.CreateReactContextFromBundleAsync(token);
            }

            public Task<ReactContext> CreateReactContextFromCachedPackagerBundleAsync(CancellationToken token)
            {
                return _parent.CreateReactContextFromCachedPackagerBundleAsync(token);
            }

            public Task<ReactContext> CreateReactContextWithRemoteDebuggerAsync(Func<IJavaScriptExecutor> javaScriptExecutorFactory, CancellationToken token)
            {
                return _parent.CreateReactContextWithRemoteDebuggerAsync(javaScriptExecutorFactory, token);
            }
        }
    }
}
