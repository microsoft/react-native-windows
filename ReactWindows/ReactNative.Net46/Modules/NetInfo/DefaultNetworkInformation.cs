using System;
using System.Net.NetworkInformation;

namespace ReactNative.Modules.NetInfo
{
    class DefaultNetworkInformation : INetworkInformation
    {
        public event NetworkAddressChangedEventHandler NetworkAddressChanged;

        public void Start()
        {
            NetworkChange.NetworkAddressChanged += OnNetworkAddressChanged;
        }

        public void Stop()
        {
            NetworkChange.NetworkAddressChanged -= OnNetworkAddressChanged;
        }

        public string GetInternetStatus()
        {
            int result;
            return NetInfoModuleNativeMethods.InternetGetConnectedState(out result, 0) ? "InternetAccess" : "None";
        }

        private void OnNetworkAddressChanged(object sender, EventArgs e)
        {
            NetworkAddressChanged?.Invoke(sender, e);
        }
    }
}
