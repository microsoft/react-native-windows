// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;

namespace ReactNative.Modules.NetInfo
{
    /// <summary>
    /// Module that monitors and provides information about the connectivity
    /// state of the device.
    /// </summary>
    public class NetInfoModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private readonly INetworkInformation _networkInfo;
        private string _connected;

        /// <summary>
        /// Instantiates the <see cref="NetInfoModule"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public NetInfoModule(ReactContext reactContext)
            : this(new DefaultNetworkInformation(), reactContext)
        {
        }

        /// <summary>
        /// Instantiates the <see cref="NetInfoModule"/>.
        /// </summary>
        /// <param name="networkInfo">The network information.</param>
        /// <param name="reactContext">The React context.</param>
        public NetInfoModule(INetworkInformation networkInfo, ReactContext reactContext)
            : base(reactContext)
        {
            _networkInfo = networkInfo;
        }

        /// <summary>
        /// Gets the name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "NetInfo";
            }
        }

        /// <summary>
        /// Gets the current connectivity state of the app.
        /// </summary>
        /// <param name="promise">A promise to resolve the request.</param>
        [ReactMethod]
        public void getCurrentConnectivity(IPromise promise)
        {
            promise.Resolve(CreateConnectivityEventMap());
        }

        /// <summary>
        /// Called when the application host is destroyed.
        /// </summary>
        public void OnDestroy()
        {
        }

        /// <summary>
        /// Called when the application host is resumed.
        /// </summary>
        public void OnResume()
        {
            _networkInfo.Start();
            _networkInfo.NetworkConnectivityChanged += OnNetworkConnectivityChanged;
        }

        /// <summary>
        /// Called when the application host is suspended.
        /// </summary>
        public void OnSuspend()
        {
            _networkInfo.NetworkConnectivityChanged -= OnNetworkConnectivityChanged;
            _networkInfo.Stop();
        }

        /// <summary>
        /// Called when the React instance is initialized.
        /// </summary>
        public override void Initialize()
        {
            Context.AddLifecycleEventListener(this);
        }

        private JObject CreateConnectivityEventMap()
        {
            return new JObject
            {
                { "network_info", _networkInfo.GetInternetStatus() },
            };
        }

        private void OnNetworkConnectivityChanged(object ignored, NetworkConnectivityChangedEventArgs e)
        {
            if (e.ConnectionStatus != _connected)
            {
                _connected = e.ConnectionStatus;
                Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("networkStatusDidChange", CreateConnectivityEventMap());
            }
        }
    }
}
