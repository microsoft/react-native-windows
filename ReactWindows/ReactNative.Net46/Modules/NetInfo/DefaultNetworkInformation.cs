using System.Net.NetworkInformation;

namespace ReactNative.Modules.NetInfo
{
    class DefaultNetworkInformation : INetworkInformation
    {
        public event NetworkAvailabilityChangedEventHandler NetworkAvailabilityChanged;

        public void Start()
        {
            NetworkChange.NetworkAvailabilityChanged += OnNetworkAvailabilityChanged;
        }

        public void Stop()
        {
            NetworkChange.NetworkAvailabilityChanged -= OnNetworkAvailabilityChanged;
        }

        public string GetInternetStatus()
        {
            return NetworkInterface.GetIsNetworkAvailable() ? "InternetAccess" : "None";
        }

        private void OnNetworkAvailabilityChanged(object sender, NetworkAvailabilityEventArgs e)
        {
            NetworkAvailabilityChanged?.Invoke(sender, e);
        }
    }
}
