// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System;
using System.Reactive.Subjects;
using System.Threading.Tasks;
using Windows.System;
using static System.FormattableString;

namespace ReactNative.Modules.Launch
{
    /// <summary>
    /// The module responsible for activating URIs and managing the initial launch URL. 
    /// </summary>
    public class LauncherModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private static readonly object s_lock = new object();
        // When using these statics, must hold `s_lock`.
        private static string s_pendingActivatedUrl;
        private static LauncherModule s_currentInstance;

        private readonly Subject<string> _urlSubject = new Subject<string>();

        private volatile string _activatedUrl;
        private bool _initialized;
        private IDisposable _subscription;

        /// <summary>
        /// Instantiates the <see cref="LauncherModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public LauncherModule(ReactContext reactContext) 
            : base(reactContext)
        {
            lock (s_lock)
            {
                _activatedUrl = s_pendingActivatedUrl;
                s_pendingActivatedUrl = null;
                s_currentInstance = this;
            }
        }

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "LinkingManager";
            }
        }

        /// <summary>
        /// Opens the given URL.
        /// </summary>
        /// <param name="url">The URL.</param>
        /// <param name="promise">
        /// The promise that should be resolved after the URL is opened.
        /// </param>
        [ReactMethod]
        public async void openURL(string url, IPromise promise)
        {
            if (url == null)
            {
                promise.Reject(new ArgumentNullException(nameof(url)));
                return;
            }

            if (!Uri.TryCreate(url, UriKind.Absolute, out var uri))
            {
                promise.Reject(new ArgumentException(Invariant($"URL argument '{uri}' is not valid.")));
                return;
            }

            try
            {
                await Launcher.LaunchUriAsync(uri).AsTask().ConfigureAwait(false);
                promise.Resolve(true);
            }
            catch (Exception ex)
            {
                promise.Reject(new InvalidOperationException(
                    Invariant($"Could not open URL '{url}'."), ex));
            }
        }

        /// <summary>
        /// Checks if the application can open the given URL.
        /// </summary>
        /// <param name="url">The URL.</param>
        /// <param name="promise">
        /// The promise used to return the result of the check.
        /// </param>
        [ReactMethod]
        public async void canOpenURL(string url, IPromise promise)
        {
            if (url == null)
            {
                promise.Reject(new ArgumentNullException(nameof(url)));
                return;
            }

            if (!Uri.TryCreate(url, UriKind.Absolute, out var uri))
            {
                promise.Reject(new ArgumentException(Invariant($"URL argument '{uri}' is not valid.")));
                return;
            }

            try
            {
                var support = await Launcher.QueryUriSupportAsync(uri, LaunchQuerySupportType.Uri).AsTask().ConfigureAwait(false);
                promise.Resolve(support == LaunchQuerySupportStatus.Available);
            }
            catch (Exception ex)
            {
                promise.Reject(new InvalidOperationException(
                    Invariant($"Could not check if URL '{url}' can be opened."), ex));
            }
        }

        /// <summary>
        /// Gets the URL the application was launched with, or <code>null</code>
        /// if the application was started normally.
        /// </summary>
        /// <param name="promise">
        /// The promise used to return the initial URL.
        /// </param>
        [ReactMethod]
        public void getInitialURL(IPromise promise)
        {
            promise.Resolve(_activatedUrl);
        }

        /// <summary>
        /// Called when the app is initialized.
        /// </summary>
        public override void Initialize()
        {
            _subscription = CreateUrlSubscription();
            _initialized = true;
        }

        /// <summary>
        /// Called when the app is being suspended.
        /// </summary>
        public void OnSuspend()
        {
            _subscription.Dispose();
        }

        /// <summary>
        /// Called when the app is resumed.
        /// </summary>
        public void OnResume()
        {
            // Wait for initialization to subscribe to protect against sending
            // events to a context that has not been fully initialized.
            if (_initialized)
            {
                _subscription = CreateUrlSubscription();
            }
        }

        /// <summary>
        /// Called when the app is destroyed.
        /// </summary>
        public void OnDestroy()
        {
        }

        /// <summary>
        /// Called before a <see cref="IReactInstance"/> is disposed.
        /// </summary>
        /// <returns>
        /// A task to await the dispose operation.
        /// </returns>
        public override Task OnReactInstanceDisposeAsync()
        {
            lock (s_lock)
            {
                if (this == s_currentInstance)
                {
                    s_currentInstance = null;
                }
            }
            return Task.CompletedTask;
        }

        /// <summary>
        /// Called whenever a protocol activation occurs.
        /// </summary>
        /// <param name="url">The URL.</param>
        public void OnActivated(string url)
        {
            _urlSubject.OnNext(url);
        }

        /// <summary>
        /// The initial URL used to activate the application.
        /// </summary>
        /// <param name="url">The URL.</param>
        public static void SetActivatedUrl(string url)
        {
            // Static state doesn't work well in RNW modules for scenarios that involve creating multiple instances of a
            // module during the lifetime of the application. Examples:
            //   1. An app creates multiple RNW bridges in which case each module will have multiple instances living at
            //      the same time.
            //   2. An app recreates the RNW context in which case each module will have multiple instances but
            //      their lifetimes won't overlap.
            // 
            // This method is designed to work for (2). The idea is that `url` is intended for 1 module instance.
            // If the instance is already around, the activated URL will be given to it. Otherwise, the module
            // instance will *consume* the activated URL during initialization. Once consumed, the URL won't
            // be around for other instances to consume during their initialization.
            //
            // The right behavior for (1) isn't clear.
            //
            // Ideally, the app would pass the activated URL directly to the appropriate instance so RNW doesn't
            // have to guess which module instance the activated URL was intended for. However, there are some
            // challenges around the right way to plumb this during initialization and some potential races of
            // getting this to the right place before JavaScript calls `getInitialURL`.

            lock (s_lock)
            {
                if (s_currentInstance != null)
                {
                    s_currentInstance._activatedUrl = url;
                }
                else
                {
                    s_pendingActivatedUrl = url;
                }
            }
        }

        private IDisposable CreateUrlSubscription()
        {
            return _urlSubject.Subscribe(url =>
                Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("url", new JObject
                    {
                        { "url", url },
                    }));
        }
    }
}
