// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

    /// <summary>
    /// Describes a network connectivity change event
    /// </summary>
    public class NetworkConnectivityChangedEventArgs : EventArgs
    {
        /// <summary>
        /// Gets or sets a value indicating whether the network is available
        /// </summary>
        public bool IsAvailable { get; set; }

        /// <summary>
        /// Gets or sets a value specifying additional network connectivity status
        /// </summary>
        public string ConnectionStatus { get; set; }
    }
}
