using System;

namespace ReactNative.Modules.NetInfo
{
    /// <summary>
    /// An interface for network information status and updates.
    /// </summary>
    public interface INetworkInformation
    {
        /// <summary>
        /// An event that occurs whenever the network connectivity changes.
        /// </summary>
        event EventHandler<NetworkConnectivityChangedEventArgs> NetworkConnectivityChanged;

        /// <summary>
        /// Gets the internet status
        /// </summary>
        /// <returns>
        /// The React Native friendly internet status
        /// </returns>
        string GetInternetStatus();

        /// <summary>
        /// Starts observing network status changes.
        /// </summary>
        void Start();

        /// <summary>
        /// Stops observing network status changes.
        /// </summary>
        void Stop();
    }

    public class NetworkConnectivityChangedEventArgs : EventArgs
    {
        public bool IsAvailable { get; set; }
        public string ConnectionStatus { get; set; }
    }
}
