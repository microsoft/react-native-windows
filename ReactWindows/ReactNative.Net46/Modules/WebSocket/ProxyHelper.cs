using ReactNative.Shared.Modules.WebSocket;
using System;
using System.Net;

namespace ReactNative.Modules.WebSocket
{
    internal class ProxyHelper
    {
        public bool IsBypassed { get; private set; } = true;
        public string ProxyAddress { get; private set; } = string.Empty;
        public string UserName { get; set; } = string.Empty;
        public string Password { get; private set; } = string.Empty;

        public ProxyHelper(Uri address)
        {
            // we get wss:// (or ws://) scheme that is not all proxies handle correctly. For better auto-discovering
            // replace wss:// to https:// (ws:// to http:// accordingly)

            var normalizedAddress = new UriBuilder(address)
            {
                Scheme = address.Scheme == "wss" ? Uri.UriSchemeHttps : Uri.UriSchemeHttp
            };

            var proxy = WebRequest.DefaultWebProxy;
            proxy.Credentials = CredentialCache.DefaultCredentials;

            IsBypassed = proxy.IsBypassed(normalizedAddress.Uri);

            if (!IsBypassed)
            {
                ProxyAddress = proxy.GetProxy(normalizedAddress.Uri).AbsoluteUri;
                var defCreds = CredentialCache.DefaultCredentials as NetworkCredential;

                if (defCreds == null)
                {
                    return;
                }

                UserName = string.IsNullOrEmpty(defCreds.Domain) ? defCreds.UserName : $"{defCreds.Domain}\\{defCreds.UserName}";
                Password = defCreds.Password;
            }
        }

        public ProxyHelper(IWebSocketOptions options)
        {
            if (options == null)
            {
                throw new ArgumentNullException(nameof(options));
            }

            if (options.UseDefaultProxy)
            {
                return;
            }

            IsBypassed = false;
            ProxyAddress = options.ProxyAddress;
            UserName = options.UserName;
            Password = options.Password;
        }
    }
}
