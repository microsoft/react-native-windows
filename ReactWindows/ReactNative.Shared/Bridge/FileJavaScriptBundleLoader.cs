using System;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Storage;
#endif

namespace ReactNative.Bridge
{
    class FileJavaScriptBundleLoader : JavaScriptBundleLoader
    {
        private string _script;

        public FileJavaScriptBundleLoader(string fileName)
        {
            SourceUrl = fileName;
        }

        public override string SourceUrl
        {
            get;
        }

        public override async Task InitializeAsync()
        {
            try
            {
#if WINDOWS_UWP
                var storageFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri(SourceUrl)).AsTask().ConfigureAwait(false);
                _script = storageFile.Path;
#else
                throw new NotImplementedException("Bundle loading of JavaScript currently only supported on Universal Windows.");
#endif
            }
            catch (Exception ex)
            {
                var exceptionMessage = FormattableString.Invariant($"File read exception for asset '{SourceUrl}'.");
                throw new InvalidOperationException(exceptionMessage, ex);
            }
        }

        public override void LoadScript(IReactBridge bridge)
        {
            if (bridge == null)
                throw new ArgumentNullException(nameof(bridge));

            if (_script == null)
            {
                throw new InvalidOperationException("Bundle loader has not yet been initialized.");
            }

            bridge.RunScript(_script, SourceUrl);
        }
    }
}