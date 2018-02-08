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
            var proxy = WebRequest.GetSystemWebProxy();
            proxy.Credentials = CredentialCache.DefaultCredentials;
            
            IsBypassed = proxy.IsBypassed(address);

            if (!IsBypassed)
            {
                ProxyAddress = proxy.GetProxy(address).AbsolutePath;
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
                throw new ArgumentNullException("options", "Parameter cannot be null");
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
