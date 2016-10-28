using System;
using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    class RemoteDebuggerJavaScriptBundleLoader : JavaScriptBundleLoader
    {
        private readonly string _proxySourceUrl;

        public RemoteDebuggerJavaScriptBundleLoader(string proxySourceUrl, string realSourceUrl)
        {
            _proxySourceUrl = proxySourceUrl;
            SourceUrl = realSourceUrl;
        }

        public override string SourceUrl
        {
            get;
        }

        public override Task InitializeAsync()
        {
            return Task.CompletedTask;
        }

        public override void LoadScript(IReactBridge executor)
        {
            if (executor == null)
                throw new ArgumentNullException(nameof(executor));

            executor.RunScript(_proxySourceUrl, SourceUrl);
        }
    }
}