using System.Threading.Tasks;
using Windows.Storage.Streams;

namespace ReactNative.Modules.Image
{
    /// <summary>
    /// An interface for a URI loader.
    /// </summary>
    public interface IUriLoader
    {
        /// <summary>
        /// Prefetch a URI.
        /// </summary>
        /// <param name="uri">The URI.</param>
        /// <returns>A task to await the prefetch operation.</returns>
        Task PrefetchAsync(string uri);

        /// <summary>
        /// Open a stream to read from a URI.
        /// </summary>
        /// <param name="uri">The URI.</param>
        /// <returns>A task to await the stream.</returns>
        Task<IRandomAccessStream> OpenReadAsync(string uri);
    }
}
