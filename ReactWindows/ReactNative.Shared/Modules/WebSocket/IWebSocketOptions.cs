namespace ReactNative.Shared.Modules.WebSocket
{
    /// <summary>
    /// Represents WebSocket options
    /// </summary>
    internal interface IWebSocketOptions
    {
        /// <summary>
        /// Gets a value indicating whether default proxy should be used for connection
        /// </summary>
        bool UseDefaultProxy { get; }

        /// <summary>
        /// Gets URI address of the proxy server that is used for connection.
        /// If UseDefaultProxy is true - this parameter should be ignored
        /// </summary>
        string ProxyAddress { get; }

        /// <summary>
        /// Gets user name for proxy authentication.
        /// If UseDefaultProxy is true - this parameter should be ignored
        /// </summary>
        string UserName { get; }

        /// <summary>
        /// Gets password for proxy authentication.
        /// If UseDefaultProxy is true - this parameter should be ignored
        /// </summary>        
        string Password { get; }

        /// <summary>
        /// Gets HTTP Header Origin
        /// </summary>
        string Origin { get; }
    }
}
