// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Networking.Connectivity;

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
        event NetworkStatusChangedEventHandler NetworkStatusChanged;

        /// <summary>
        /// Gets the connection profile associated with the internet connection
        /// currently used by the local machine.
        /// </summary>
        /// <returns>
        /// The profile for the connection currently used to connect the machine
        /// to the Internet, or null if there is no connection profile with a 
        /// suitable connection.
        /// </returns>
        IConnectionProfile GetInternetConnectionProfile();

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
