using System;
using System.Threading.Tasks;
using PCLStorage;

namespace ReactNative.Bridge
{
    class CachedJavaScriptBundleLoader : JavaScriptBundleLoader
    {
        private readonly string _cachedFileLocation;
        private string _script;

        public CachedJavaScriptBundleLoader(string sourceUrl, string cachedFileLocation)
        {
            SourceUrl = sourceUrl;
            _cachedFileLocation = cachedFileLocation;
        }

        public override string SourceUrl { get; }

        public override async Task InitializeAsync()
        {
            try
            {
                var localFolder = FileSystem.Current.LocalStorage;
                var storageFile = await localFolder.GetFileAsync(_cachedFileLocation).ConfigureAwait(false);
                _script = storageFile.Path;
            }
            catch (Exception ex)
            {
                var exceptionMessage = FormattableString.Invariant($"File read exception for asset '{SourceUrl}'.");
                throw new InvalidOperationException(exceptionMessage, ex);
            }
        }

        public override void LoadScript(IReactBridge executor)
        {
            if (executor == null)
                throw new ArgumentNullException(nameof(executor));

            executor.RunScript(_script, SourceUrl);
        }
    }
}