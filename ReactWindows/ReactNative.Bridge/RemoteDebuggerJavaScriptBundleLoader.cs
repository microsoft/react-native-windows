using System;
using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    public class RemoteDebuggerJavaScriptBundleLoader : IJavaScriptBundleLoader
    {
        private static readonly Task CompletedTask = Task.FromResult(true);
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
            return CompletedTask;
        }

        public void LoadScript(IReactBridge bridge)
        {
            if (bridge == null) throw new ArgumentNullException(nameof(bridge));

            bridge.RunScript(_proxySourceUrl, SourceUrl);
        }
    }
}