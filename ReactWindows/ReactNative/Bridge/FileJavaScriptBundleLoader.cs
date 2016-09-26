using System;
using System.Threading.Tasks;
using Windows.Storage;

namespace ReactNative.Bridge
{
    class FileJavaScriptBundleLoader : IJavaScriptBundleLoader
    {
        private string _script;

        public FileJavaScriptBundleLoader(string fileName)
        {
            SourceUrl = fileName;
        }

        public string SourceUrl
        {
            get;
        }

        public async Task InitializeAsync()
        {
            try
            {
                var storageFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri(SourceUrl)).AsTask().ConfigureAwait(false);
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