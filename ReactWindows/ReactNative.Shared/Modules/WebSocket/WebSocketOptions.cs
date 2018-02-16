using Newtonsoft.Json.Linq;
using ReactNative.Collections;
using ReactNative.Shared.Modules.WebSocket;
using System;

namespace ReactNative.Modules.WebSocket
{
    internal class WebSocketOptions : IWebSocketOptions
    {
        private static readonly string HeadersSection = "headers";
        private static readonly string HeadersOrigin = "origin";
        private static readonly string ProxySection = "proxy";
        private static readonly string ProxyUseDefault = "useDefault";
        private static readonly string ProxyUri = "uri";
        private static readonly string ProxyUser = "user";
        private static readonly string ProxyPassword = "password";

        public WebSocketOptions(JObject options)
        {
            if (options == null)
            {
                return;
            }

            if (options.ContainsKey(ProxySection))
            {
                var proxy = options[ProxySection];

                UseDefaultProxy = (bool)(proxy.SelectToken(ProxyUseDefault, false) ?? false);
                ProxyAddress = (string)(proxy.SelectToken(ProxyUri, false) ?? string.Empty);
                UserName = (string)(proxy.SelectToken(ProxyUser, false) ?? string.Empty);
                Password = (string)(proxy.SelectToken(ProxyPassword, false) ?? string.Empty);
            }

            if (options.ContainsKey(HeadersSection))
            {
                var headers = options[HeadersSection];

                Origin = (string)(headers.SelectToken(HeadersOrigin, false) ?? string.Empty);
            }
        }

        #region IWebSocketOptions

        /// <inheritdoc />
        public bool UseDefaultProxy { get; private set; } = false;

        /// <inheritdoc />
        public string ProxyAddress { get; private set; } = string.Empty;

        /// <inheritdoc />
        public string UserName { get; private set; } = string.Empty;

        /// <inheritdoc />
        public string Password { get; private set; } = string.Empty;

        /// <inheritdoc />
        public string Origin { get; private set; } = string.Empty;
        #endregion
    }
}
