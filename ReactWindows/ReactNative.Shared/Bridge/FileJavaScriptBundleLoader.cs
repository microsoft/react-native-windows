using System;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Storage;
#else
using System.IO;
using System.Reflection;
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
                var assembly = Assembly.GetAssembly(typeof(FileJavaScriptBundleLoader));
                var assemblyName = assembly.GetName();
                var pathToAssembly = Path.GetDirectoryName(assemblyName.CodeBase);
                var pathToAssemblyResource = Path.Combine(pathToAssembly, SourceUrl.Replace("ms-appx:///", String.Empty));
                var u = new Uri(pathToAssemblyResource);
                _script = u.LocalPath;
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