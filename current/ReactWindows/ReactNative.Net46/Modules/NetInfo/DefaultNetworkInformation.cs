// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NETWORKLIST;
using System;

namespace ReactNative.Modules.NetInfo
{
    class DefaultNetworkInformation : INetworkInformation
    {
        public event EventHandler<NetworkConnectivityChangedEventArgs> NetworkConnectivityChanged;
        private NetworkListManager _networkListManager;

        public void Start()
        {
            _networkListManager = new NetworkListManager();
            _networkListManager.NetworkConnectivityChanged += OnNetworkConnectivityChanged;
        }

        public void Stop()
        {
            _networkListManager.NetworkConnectivityChanged -= OnNetworkConnectivityChanged;
            _networkListManager = null;
        }

        public string GetInternetStatus()
        {
            return _networkListManager.IsConnectedToInternet ? "InternetAccess" : "None";
        }

        private void OnNetworkConnectivityChanged(Guid guid, NLM_CONNECTIVITY connectivity)
        {
            NetworkConnectivityChangedEventArgs e = new NetworkConnectivityChangedEventArgs()
            {
                IsAvailable = _networkListManager.IsConnectedToInternet,
                ConnectionStatus = GetInternetStatus()
            };
            NetworkConnectivityChanged?.Invoke(new object(), e);
        }
    }
}
