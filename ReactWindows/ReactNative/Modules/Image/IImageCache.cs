namespace ReactNative.Modules.Image
{
    /// <summary>
    /// An interface for an image cache.
    /// </summary>
    public interface IImageCache
    {
        /// <summary>
        /// Gets a reference to an image from the given URI.
        /// </summary>
        /// <param name="uri">The URI.</param>
        /// <returns>The image reference.</returns>
        IImageReference Get(string uri);
    }
}
