using System;
using System.Threading.Tasks;
using Windows.Storage.Streams;

namespace ReactNative.Modules.Image
{
    class DefaultUriLoader : IUriLoader
    {
        public async Task<IRandomAccessStreamWithContentType> OpenReadAsync(string uri)
        {
            var streamRef = RandomAccessStreamReference.CreateFromUri(new Uri(uri));
            return await streamRef.OpenReadAsync().AsTask().ConfigureAwait(false);
        }

        public Task PrefetchAsync(string uri)
        {
            throw new NotImplementedException("Prefetch is not yet implemented.");
        }
    }
}
