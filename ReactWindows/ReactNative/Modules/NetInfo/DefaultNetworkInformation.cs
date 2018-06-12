// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Networking.Connectivity;

namespace ReactNative.Modules.NetInfo
{
    class DefaultNetworkInformation : INetworkInformation
    {
        public event NetworkStatusChangedEventHandler NetworkStatusChanged;

        public void Start()
        {
            NetworkInformation.NetworkStatusChanged += OnNetworkStatusChanged;
        }

        public void Stop()
        {
            NetworkInformation.NetworkStatusChanged -= OnNetworkStatusChanged;
        }

        public IConnectionProfile GetInternetConnectionProfile()
        {
            var profile = NetworkInformation.GetInternetConnectionProfile();
            return profile != null
                ? new ConnectionProfileImpl(profile)
                : null;
        }

        private void OnNetworkStatusChanged(object sender)
        {
            NetworkStatusChanged?.Invoke(sender);
        }

        class ConnectionProfileImpl : IConnectionProfile
        {
            private readonly ConnectionProfile _profile;

            // Based on the EffectiveConnectionType enum described in the W3C Network Information API spec
            // (https://wicg.github.io/netinfo/).

            private string EFFECTIVE_CONNECTION_TYPE_UNKNOWN = "unknown";

            private string EFFECTIVE_CONNECTION_TYPE_2G = "2g";

            private string EFFECTIVE_CONNECTION_TYPE_3G = "3g";

            private string EFFECTIVE_CONNECTION_TYPE_4G = "4g";

            public ConnectionProfileImpl(ConnectionProfile profile)
            {
                _profile = profile;
            }

            public NetworkConnectivityLevel ConnectivityLevel
            {
                get
                {
                    return _profile.GetNetworkConnectivityLevel();
                }
            }

            public string ConnectivityType
            {
                get
                {
                   WwanDataClass effectiveConnnectivityType = _profile.IsWwanConnectionProfile ? _profile.WwanConnectionProfileDetails.GetCurrentDataClass() : WwanDataClass.None;
                    switch (effectiveConnnectivityType)
                    {
                        //2G-equivalent
                        case WwanDataClass.Edge:
                        case WwanDataClass.Gprs:
                            return EFFECTIVE_CONNECTION_TYPE_2G;
                        //3G-equivalent
                        case WwanDataClass.Cdma1xEvdo:
                        case WwanDataClass.Cdma1xEvdoRevA:
                        case WwanDataClass.Cdma1xEvdoRevB:
                        case WwanDataClass.Cdma1xEvdv:
                        case WwanDataClass.Cdma1xRtt:
                        case WwanDataClass.Cdma3xRtt:
                        case WwanDataClass.CdmaUmb:
                        case WwanDataClass.Umts:
                        case WwanDataClass.Hsdpa:
                        case WwanDataClass.Hsupa:
                            return EFFECTIVE_CONNECTION_TYPE_3G;
                        //4G-equivalent
                        case WwanDataClass.LteAdvanced:
                            return EFFECTIVE_CONNECTION_TYPE_4G;
                        default:
                            return EFFECTIVE_CONNECTION_TYPE_UNKNOWN;
                    }
                }
            }
        }
    }
}
