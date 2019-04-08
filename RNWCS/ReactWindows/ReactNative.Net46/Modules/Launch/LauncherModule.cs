// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System;
using System.Reactive.Subjects;
using static System.FormattableString;

namespace ReactNative.Modules.Launch
{
    /// <summary>
    /// The module responsible for activating URIs and managing the initial launch URL. 
    /// </summary>
    public class LauncherModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private static readonly Subject<string> s_urlSubject = new Subject<string>();
        private static string s_activatedUrl;

        private bool _initialized;
        private IDisposable _subscription;

        /// <summary>
        /// Instantiates the <see cref="LauncherModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public LauncherModule(ReactContext reactContext)
            : base(reactContext)
        {
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
        public void openURL(string url, IPromise promise)
        {
            if (url == null)
            {
                promise.Reject(new ArgumentNullException(nameof(url)));
                return;
            }

            var uri = default(Uri);
            if (!Uri.TryCreate(url, UriKind.Absolute, out uri))
            {
                promise.Reject(new ArgumentException(Invariant($"URL argument '{uri}' is not valid.")));
                return;
            }

            System.Diagnostics.Process.Start(uri.AbsoluteUri);
            promise.Resolve(true);
        }

        /// <summary>
        /// Checks if the application can open the given URL.
        /// </summary>
        /// <param name="url">The URL.</param>
        /// <param name="promise">
        /// The promise used to return the result of the check.
        /// </param>
        [ReactMethod]
        public void canOpenURL(string url, IPromise promise)
        {
            if (url == null)
            {
                promise.Reject(new ArgumentNullException(nameof(url)));
                return;
            }

            var uri = default(Uri);
            if (!Uri.TryCreate(url, UriKind.Absolute, out uri))
            {
                promise.Reject(new ArgumentException(Invariant($"URL argument '{uri}' is not valid.")));
                return;
            }

            promise.Resolve(true);
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
            promise.Resolve(s_activatedUrl);
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
            _subscription.Dispose();
        }

        /// <summary>
        /// The initial URL used to activate the application.
        /// </summary>
        public static void SetActivatedUrl(string url)
        {
            s_activatedUrl = url;
            s_urlSubject.OnNext(url);
        }

        private IDisposable CreateUrlSubscription()
        {
            return s_urlSubject.Subscribe(url =>
                Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("url", new JObject
                    {
                        { "url", url },
                    }));
        }
    }
}
