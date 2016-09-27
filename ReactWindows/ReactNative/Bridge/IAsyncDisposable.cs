using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A resource that can be disposed asynchronously.
    /// </summary>
    public interface IAsyncDisposable
    {
        /// <summary>
        /// Asynchronously disposes the instance.
        /// </summary>
        /// <returns>A task to await dispose operation.</returns>
        Task DisposeAsync();
    }
}
