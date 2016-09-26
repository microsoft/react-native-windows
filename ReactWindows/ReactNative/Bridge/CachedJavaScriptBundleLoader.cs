using System;
using System.Threading.Tasks;
using Windows.Storage;

namespace ReactNative.Bridge
{
    class CachedJavaScriptBundleLoader : IJavaScriptBundleLoader
    {
        private readonly string _cachedFileLocation;
        private string _script;

        public CachedJavaScriptBundleLoader(string sourceUrl, string cachedFileLocation)
        {
            SourceUrl = sourceUrl;
            _cachedFileLocation = cachedFileLocation;
        }

        public string SourceUrl { get; }

        public async Task InitializeAsync()
        {
            try
            {
                var localFolder = ApplicationData.Current.LocalFolder;
                var storageFile = await localFolder.GetFileAsync(_cachedFileLocation).AsTask().ConfigureAwait(false);
                _script = storageFile.Path;
            }
            catch (Exception ex)
            {
                var exceptionMessage = FormattableString.Invariant($"File read exception for asset '{SourceUrl}'.");
                throw new InvalidOperationException(exceptionMessage, ex);
            }
        }

        public void LoadScript(IReactBridge bridge)
        {
            if (bridge == null) throw new ArgumentNullException(nameof(bridge));

            if (_script == null) throw new InvalidOperationException("Bundle loader has not yet been initialized.");

            bridge.RunScript(_script, SourceUrl);
        }
    }
}