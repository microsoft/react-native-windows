using System;
using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    class RemoteDebuggerJavaScriptBundleLoader : IJavaScriptBundleLoader
    {
        private readonly string _proxySourceUrl;

        public RemoteDebuggerJavaScriptBundleLoader(string proxySourceUrl, string realSourceUrl)
        {
            _proxySourceUrl = proxySourceUrl;
            SourceUrl = realSourceUrl;
        }

        public string SourceUrl
        {
            get;
        }

        public Task InitializeAsync()
        {
            return Task.CompletedTask;
        }

        public void LoadScript(IReactBridge executor)
        {
            if (executor == null)
                throw new ArgumentNullException(nameof(executor));

            executor.RunScript(_proxySourceUrl, SourceUrl);
        }
    }
}