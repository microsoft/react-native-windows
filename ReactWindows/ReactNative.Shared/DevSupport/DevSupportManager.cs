using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.Modules.DevSupport;
using ReactNative.Tracing;
using System;
using System.IO;
using System.Reactive.Disposables;
using System.Runtime.ExceptionServices;
using System.Threading;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Storage;
#else
using PCLStorage;
using System.Reflection;
using System.Windows;
#endif

namespace ReactNative.DevSupport
{
    class DevSupportManager : IDevSupportManager, IDisposable
    {
        private const int NativeErrorCookie = -1;
        private const string JSBundleFileName = "ReactNativeDevBundle.js";

#if WINDOWS_UWP
        private readonly ShakeAccelerometer _accelerometer = ShakeAccelerometer.Instance;
        private bool _isShakeDetectorRegistered;
#endif

        private readonly SerialDisposable _pollingDisposable = new SerialDisposable();

        private readonly IReactInstanceDevCommandsHandler _reactInstanceCommandsHandler;
        private readonly bool _shouldLoadFromPackagerServer;
        private readonly string _jsAppBundleName;
        private readonly DevInternalSettings _devSettings;
        private readonly DevServerHelper _devServerHelper;

        private bool _isDevSupportEnabled = true;

        private ReactContext _currentReactContext;
        private RedBoxDialog _redBoxDialog;
        private Action _dismissRedBoxDialog;
        private bool _redBoxDialogOpen;
        private DevOptionDialog _devOptionsDialog;
        private Action _dismissDevOptionsDialog;
        private bool _devOptionsDialogOpen;

        public DevSupportManager(
            IReactInstanceDevCommandsHandler reactInstanceCommandsHandler,
            bool shouldLoadFromPackagerServer,
            string jsAppBundleName)
        {
            _reactInstanceCommandsHandler = reactInstanceCommandsHandler;
            _shouldLoadFromPackagerServer = shouldLoadFromPackagerServer;
            _jsAppBundleName = jsAppBundleName;
            _devSettings = new DevInternalSettings(this);
            _devServerHelper = new DevServerHelper(_devSettings);
            ReloadSettings();
        }

        public IDeveloperSettings DevSettings
        {
            get
            {
                return _devSettings;
            }
        }

        public string DownloadedJavaScriptBundleFile
        {
            get
            {
                return JSBundleFileName;
            }
        }

        public bool IsEnabled
        {
            get
            {
                return _isDevSupportEnabled;
            }
            set
            {
                if (value != _isDevSupportEnabled)
                {
                    _isDevSupportEnabled = value;
                    ReloadSettings();
                }
            }
        }

        public bool IsRemoteDebuggingEnabled
        {
            get;
            set;
        }

        public bool IsProgressDialogEnabled
        {
            get;
            set;
        } = true;

        public string SourceMapUrl
        {
            get
            {
                if (_jsAppBundleName == null)
                {
                    return "";
                }

                return _devServerHelper.GetSourceMapUrl(_jsAppBundleName);
            }
        }

        public string SourceUrl
        {
            get
            {
                if (_jsAppBundleName == null)
                {
                    return "";
                }

                return _devServerHelper.GetSourceUrl(_jsAppBundleName);
            }
        }

        public string JavaScriptBundleUrlForRemoteDebugging
        {
            get
            {
                return _devServerHelper.GetJavaScriptBundleUrlForRemoteDebugging(_jsAppBundleName);
            }
        }

        public void HandleException(Exception exception)
        {
            if (IsEnabled)
            {
                ShowNewNativeError(exception.Message, exception);
            }
            else
            {
                ExceptionDispatchInfo.Capture(exception).Throw();
            }
        }

        public bool HasUpToDateBundleInCache()
        {
            if (_isDevSupportEnabled)
            {
#if WINDOWS_UWP
                var lastUpdateTime = Windows.ApplicationModel.Package.Current.InstalledDate.UtcDateTime;
                var localFolder = ApplicationData.Current.LocalFolder.Path;
#else
                var lastUpdateTime = File.GetCreationTime(Assembly.GetExecutingAssembly().Location);
                var localFolder = FileSystem.Current.LocalStorage.Path;
#endif
                var localFileName = Path.Combine(localFolder, JSBundleFileName);
                if (File.Exists(localFileName))
                {
                    return File.GetLastWriteTimeUtc(localFileName) > lastUpdateTime;
                }
            }

            return false;
        }

        public void ShowNewNativeError(string message, Exception exception)
        {
            if (exception is JavaScriptException javaScriptException && javaScriptException.JavaScriptStackTrace != null)
            {
                var stackTrace = StackTraceHelper.ConvertChakraStackTrace(javaScriptException.JavaScriptStackTrace);
                ShowNewError(exception.Message, stackTrace, NativeErrorCookie);
            }
            else
            {
                Tracer.Error(ReactConstants.Tag, "Exception in native call from JavaScript.", exception);
                ShowNewError(message, StackTraceHelper.ConvertNativeStackTrace(exception), NativeErrorCookie);
            }
        }

        public void ShowNewJavaScriptError(string title, JArray details, int errorCookie)
        {
            ShowNewError(title, StackTraceHelper.ConvertJavaScriptStackTrace(details), errorCookie);
        }

        public void UpdateJavaScriptError(string message, JArray details, int errorCookie)
        {
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                if (_redBoxDialog == null
                    || !_redBoxDialogOpen
                    || errorCookie != _redBoxDialog.ErrorCookie)
                {
                    return;
                }

                _redBoxDialog.Message = message;
                _redBoxDialog.StackTrace = StackTraceHelper.ConvertJavaScriptStackTrace(details);
            });
        }

        public void HideRedboxDialog()
        {
            var dismissRedBoxDialog = _dismissRedBoxDialog;
            if (_redBoxDialogOpen && dismissRedBoxDialog != null)
            {
                dismissRedBoxDialog();
            }
        }

        public void ShowDevOptionsDialog()
        {
            if (_devOptionsDialog != null || !IsEnabled)
            {
                return;
            }

            DispatcherHelpers.RunOnDispatcher(() =>
            {
                var options = new[]
                {
                    new DevOptionHandler(
                        "Reload JavaScript",
                        HandleReloadJavaScript),
                    new DevOptionHandler(
                        IsRemoteDebuggingEnabled
                            ? "Stop JS Remote Debugging"
                            : "Start JS Remote Debugging",
                        () =>
                        {
                            IsRemoteDebuggingEnabled = !IsRemoteDebuggingEnabled;
                            HandleReloadJavaScript();
                        }),
                    new DevOptionHandler(
                        _devSettings.IsHotModuleReplacementEnabled
                            ? "Disable Hot Reloading"
                            : "Enable Hot Reloading",
                        () =>
                        {
                            _devSettings.IsHotModuleReplacementEnabled = !_devSettings.IsHotModuleReplacementEnabled;
                            HandleReloadJavaScript();
                        }),
                    new DevOptionHandler(
                        _devSettings.IsReloadOnJavaScriptChangeEnabled
                            ? "Disable Live Reload"
                            : "Enable Live Reload",
                        () =>
                            _devSettings.IsReloadOnJavaScriptChangeEnabled =
                                !_devSettings.IsReloadOnJavaScriptChangeEnabled),
                    new DevOptionHandler(
                        _devSettings.IsElementInspectorEnabled
                            ? "Hide Inspector"
                            : "Show Inspector",
                        () =>
                        {
                            _devSettings.IsElementInspectorEnabled = !_devSettings.IsElementInspectorEnabled;
                            _currentReactContext?
                                .GetJavaScriptModule<RCTDeviceEventEmitter>()
                                .emit("toggleElementInspector", null);
                        }),
                };

                _devOptionsDialogOpen = true;
                _devOptionsDialog = new DevOptionDialog();
                _devOptionsDialog.Closed += (_, __) =>
                {
                    _devOptionsDialogOpen = false;
                    _dismissDevOptionsDialog = null;
                    _devOptionsDialog = null;
                };

                foreach (var option in options)
                {
                    _devOptionsDialog.Add(option.Name, option.OnSelect);
                }

                if (_redBoxDialog != null)
                {
                    _dismissRedBoxDialog();
                }

#if WINDOWS_UWP
                var asyncInfo = _devOptionsDialog.ShowAsync();
                _dismissDevOptionsDialog = asyncInfo.Cancel;

                foreach (var option in options)
                {
                    option.HideDialog = _dismissDevOptionsDialog;
                }
#else
                if (Application.Current != null && Application.Current.MainWindow != null && Application.Current.MainWindow.IsLoaded)
                {
                    _devOptionsDialog.Owner = Application.Current.MainWindow;
                }
                else
                {
                    _devOptionsDialog.Topmost = true;
                    _devOptionsDialog.WindowStartupLocation = WindowStartupLocation.CenterScreen;
                }

                _dismissDevOptionsDialog = _devOptionsDialog.Close;
                _devOptionsDialog.Show();

                foreach (var option in options)
                {
                    option.HideDialog = _dismissDevOptionsDialog;
                }
#endif
            });
        }

        private void HideDevOptionsDialog()
        {
            var dismissDevOptionsDialog = _dismissDevOptionsDialog;
            if (_devOptionsDialogOpen && dismissDevOptionsDialog != null)
            {
                dismissDevOptionsDialog();
            }
        }

        public void OnNewReactContextCreated(ReactContext context)
        {
            ResetCurrentContext(context);
        }

        public void OnReactContextDestroyed(ReactContext context)
        {
            if (context == _currentReactContext)
            {
                ResetCurrentContext(null);
            }
        }

        public Task<bool> IsPackagerRunningAsync(CancellationToken token)
        {
            return _devServerHelper.IsPackagerRunningAsync(token);
        }

        public Task<ReactContext> CreateReactContextFromPackagerAsync(CancellationToken token)
        {
            DispatcherHelpers.AssertOnDispatcher();

            HideRedboxDialog();
            HideDevOptionsDialog();

            if (IsRemoteDebuggingEnabled)
            {
                return ReloadJavaScriptInProxyModeAsync(token);
            }
            else if (_shouldLoadFromPackagerServer)
            {
                return ReloadJavaScriptFromServerAsync(token);
            }
            else
            {
                return _reactInstanceCommandsHandler.CreateReactContextFromBundleAsync(token);
            }
        }

        public void ReloadSettings()
        {
            if (_isDevSupportEnabled)
            {
#if WINDOWS_UWP
                RegisterDevOptionsMenuTriggers();
#endif
                if (_devSettings.IsReloadOnJavaScriptChangeEnabled)
                {
                    _pollingDisposable.Disposable =
                        _devServerHelper.StartPollingOnChangeEndpoint(HandleReloadJavaScript);
                }
                else
                {
                    // Disposes any existing poller
                    _pollingDisposable.Disposable = Disposable.Empty;
                }
            }
            else
            {
#if WINDOWS_UWP
                UnregisterDevOptionsMenuTriggers();
#endif

                if (_redBoxDialog != null)
                {
                    _dismissRedBoxDialog();
                }

                _pollingDisposable.Disposable = Disposable.Empty;
            }
        }

        public void Dispose()
        {
            _pollingDisposable.Dispose();
            _devServerHelper.Dispose();
        }

        public async void HandleReloadJavaScript()
        {
            using (await _reactInstanceCommandsHandler.LockAsync())
            {
                await CreateReactContextFromPackagerAsync(CancellationToken.None);
            }
        }

        private ProgressDialog CreateProgressDialog(string message)
        {
            if (IsProgressDialogEnabled)
            {
                var progressDialog = new ProgressDialog("Please wait...", message);

#if !WINDOWS_UWP
                if (Application.Current != null && Application.Current.MainWindow != null && Application.Current.MainWindow.IsLoaded)
                {
                    progressDialog.Owner = Application.Current.MainWindow;
                }
                else
                {
                    progressDialog.Topmost = true;
                    progressDialog.WindowStartupLocation = WindowStartupLocation.CenterScreen;
                }
#endif

                return progressDialog;
            }
            else
            {
                return null;
            }
        }

        private Action ShowProgressDialog(ProgressDialog progressDialog)
        {
#if WINDOWS_UWP
            var operation = progressDialog.ShowAsync();
            return operation.Cancel;
#else
            progressDialog.Show();
            return progressDialog.Close;
#endif
        }

        private void ResetCurrentContext(ReactContext context)
        {
            if (_currentReactContext == context)
            {
                return;
            }

            _currentReactContext = context;

            if (_devSettings.IsHotModuleReplacementEnabled && context != null)
            {
                var uri = new Uri(SourceUrl);
                var path = uri.LocalPath.Substring(1); // strip initial slash in path
                var host = uri.Host;
                var port = uri.Port;
                context.GetJavaScriptModule<HMRClient>().enable("windows", path, host, port);
            }
        }

        private void ShowNewError(string message, IStackFrame[] stack, int errorCookie)
        {
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                if (_redBoxDialog == null)
                {
                    _redBoxDialog = new RedBoxDialog(HandleReloadJavaScript);
                }

                if (_redBoxDialogOpen)
                {
                    return;
                }

                _redBoxDialogOpen = true;
                _redBoxDialog.ErrorCookie = errorCookie;
                _redBoxDialog.Message = message;
                _redBoxDialog.StackTrace = stack;
                _redBoxDialog.Closed += (_, __) =>
                {
                    _redBoxDialogOpen = false;
                    _dismissRedBoxDialog = null;
                    _redBoxDialog = null;
                };

#if WINDOWS_UWP
                var asyncInfo = _redBoxDialog.ShowAsync();
                _dismissRedBoxDialog = asyncInfo.Cancel;
#else
                if (Application.Current != null && Application.Current.MainWindow != null && Application.Current.MainWindow.IsLoaded)
                {
                    _redBoxDialog.Owner = Application.Current.MainWindow;
                }
                else
                {
                    _redBoxDialog.Topmost = true;
                    _redBoxDialog.WindowStartupLocation = WindowStartupLocation.CenterScreen;
                }

                _dismissRedBoxDialog = _redBoxDialog.Close;
                _redBoxDialog.ShowDialog();
#endif
            });
        }

        private async Task DownloadBundleFromPackagerAsync(CancellationToken token)
        {
            var deleteTemporaryFile = false;
#if WINDOWS_UWP
            var temporaryFolder = ApplicationData.Current.TemporaryFolder.Path;
            var localFolder = ApplicationData.Current.LocalFolder.Path;
#else
            var temporaryFolder = Path.GetTempPath();
            var localFolder = FileSystem.Current.LocalStorage.Path;
#endif
            var temporaryFilePath = Path.Combine(temporaryFolder, JSBundleFileName);

            try
            {
                using (var stream = File.OpenWrite(temporaryFilePath))
                {
                    deleteTemporaryFile = true;
                    await _devServerHelper.DownloadBundleFromUrlAsync(_jsAppBundleName, stream, token).ConfigureAwait(false);
                }

                var localFilePath = Path.Combine(localFolder, JSBundleFileName);
                if (File.Exists(localFilePath))
                {
                    File.Delete(localFilePath);
                }

                File.Move(temporaryFilePath, localFilePath);
                deleteTemporaryFile = false;
            }
            finally
            {
                if (deleteTemporaryFile)
                {
                    File.Delete(temporaryFilePath);
                }
            }
        }

        private async Task<ReactContext> ReloadJavaScriptInProxyModeAsync(CancellationToken token)
        {
            var webSocketExecutor = default(WebSocketJavaScriptExecutor);
            try
            {
                var progressDialog = CreateProgressDialog("Connecting to remote debugger.");
                var dismissed = await RunWithProgressAsync(
                    async progressToken =>
                    {
                        await _devServerHelper.LaunchDevToolsAsync(progressToken).ConfigureAwait(false);
                        webSocketExecutor = new WebSocketJavaScriptExecutor();
                        await webSocketExecutor.ConnectAsync(_devServerHelper.WebsocketProxyUrl, progressToken).ConfigureAwait(false);
                    },
                    progressDialog,
                    token);
            }
            catch (OperationCanceledException)
            when (token.IsCancellationRequested)
            {
                token.ThrowIfCancellationRequested();
            }
            catch (DebugServerException ex)
            {
                ShowNewNativeError(ex.Message, ex);
                return null;
            }
            catch (Exception ex)
            {
                ShowNewNativeError(
                    "Unable to connect to remote debugger. Did you forget " +
                    "to start the development server or connect your device?",
                    ex);
                return null;
            }

            return await _reactInstanceCommandsHandler.CreateReactContextWithRemoteDebuggerAsync(() => webSocketExecutor, token);
        }

        private async Task<ReactContext> ReloadJavaScriptFromServerAsync(CancellationToken token)
        {
            try
            {
                var progressDialog = CreateProgressDialog("Fetching JavaScript bundle.");
                var dismissed = await RunWithProgressAsync(
                    progressToken => DownloadBundleFromPackagerAsync(progressToken),
                    progressDialog,
                    token);
                if (dismissed)
                {
                    return null;
                }
            }
            catch (OperationCanceledException)
            when (token.IsCancellationRequested)
            {
                token.ThrowIfCancellationRequested();
            }
            catch (DebugServerException ex)
            {
                ShowNewNativeError(ex.Message, ex);
                return null;
            }
            catch (Exception ex)
            {
                ShowNewNativeError(
                    "Unable to download JS bundle. Did you forget to start " +
                    "the development server or connect your device?",
                    ex);
                return null;
            }

            return await _reactInstanceCommandsHandler.CreateReactContextFromCachedPackagerBundleAsync(token);
        }

        private async Task<bool> RunWithProgressAsync(Func<CancellationToken, Task> asyncAction, ProgressDialog progressDialog, CancellationToken token)
        {
            DispatcherHelpers.AssertOnDispatcher();
            var hideProgress = ShowProgressDialog(progressDialog);
            using (var cancellationDisposable = new CancellationDisposable())
            using (token.Register(cancellationDisposable.Dispose))
            using (progressDialog.Token.Register(cancellationDisposable.Dispose))
            {
                try
                {
                    await asyncAction(cancellationDisposable.Token);
                }
                catch (OperationCanceledException)
                when (progressDialog.Token.IsCancellationRequested)
                {
                    return true;
                }
                catch (OperationCanceledException)
                {
                    token.ThrowIfCancellationRequested();
                    throw;
                }
                finally
                {
                    hideProgress();
                }
            }

            return false;
        }

#if WINDOWS_UWP
        private void RegisterDevOptionsMenuTriggers()
        {
            if (!_isShakeDetectorRegistered && _accelerometer != null)
            {
                _isShakeDetectorRegistered = true;
                _accelerometer.Shaken += OnAccelerometerShake;
            }
        }

        private void UnregisterDevOptionsMenuTriggers()
        {
            if (_isShakeDetectorRegistered && _accelerometer != null)
            {
                _accelerometer.Shaken -= OnAccelerometerShake;
                _isShakeDetectorRegistered = false;
            }
        }

        private void OnAccelerometerShake(object sender, EventArgs args)
        {
            ShowDevOptionsDialog();
        }
#endif

        class DevOptionHandler
        {
            private readonly Action _onSelect;

            public DevOptionHandler(string name, Action onSelect)
            {
                Name = name;
                _onSelect = onSelect;
            }

            public string Name { get; }

            public Action HideDialog { get; set; }

            public void OnSelect()
            {
                HideDialog?.Invoke();

                _onSelect();
            }
        }
    }
}
