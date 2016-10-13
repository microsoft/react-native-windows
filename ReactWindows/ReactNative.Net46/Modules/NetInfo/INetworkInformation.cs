using System.Net.NetworkInformation;

namespace ReactNative.Modules.NetInfo
{
    /// <summary>
    /// An interface for network information status and updates.
    /// </summary>
    public interface INetworkInformation
    {
        /// <summary>
        /// An event that occurs whenever the network status changes.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1009:DeclareEventHandlersCorrectly", Justification = "API matches Windows.Networking.Connectivity.NetworkingInformation.")]
        event NetworkAvailabilityChangedEventHandler NetworkAvailabilityChanged;

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
}
