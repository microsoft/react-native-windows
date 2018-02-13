using ReactNative.Bridge;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative
{
    /// <summary>
    /// Extension methods for the React instance manager.
    /// </summary>
    public static class ReactInstanceManagerExtensions
    {
        /// <summary>
        /// Gets or creates the React context.
        /// </summary>
        /// <param name="manager">The React instance manager.</param>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>
        /// A task to await the React context.
        /// </returns>
        public static Task<ReactContext> GetOrCreateReactContextAsync(this ReactInstanceManager manager, CancellationToken token)
        {
            if (manager.HasStartedCreatingInitialContext)
            {
                return manager.GetReactContextAsync(token);
            }

            return manager.CreateReactContextAsync(token);
        }
    }
}
