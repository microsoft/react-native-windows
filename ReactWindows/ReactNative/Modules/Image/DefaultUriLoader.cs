using System;
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Storage.Streams;

namespace ReactNative.Modules.Image
{
    class DefaultUriLoader : IUriLoader
    {
        public async Task<IRandomAccessStream> OpenReadAsync(string uri)
        {
            IRandomAccessStream stream;

            if (uri.StartsWith("data:"))
            {
                var decodedData = Convert.FromBase64String(uri.Substring(uri.IndexOf(",") + 1));
                stream = decodedData.AsBuffer().AsStream().AsRandomAccessStream();
            }
            else
            {
                var streamRef = RandomAccessStreamReference.CreateFromUri(new Uri(uri));
                stream = await streamRef.OpenReadAsync();
            }

            return stream;
        }

        public Task PrefetchAsync(string uri)
        {
            throw new NotImplementedException("Pre-fetch is not yet implemented.");
        }
    }
}
