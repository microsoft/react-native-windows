using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Stores JavaScript bundle information and allows the
    /// <see cref="IReactInstance"/> to load a correct bundle through the
    /// <see cref="IReactBridge"/>.
    /// </summary>
    public interface IJavaScriptBundleLoader
    {
        /// <summary>
        /// The source URL of the bundle.
        /// </summary>
        string SourceUrl { get; }

        /// <summary>
        /// Initializes the JavaScript bundle loader.
        /// </summary>
        /// <returns>A task to await initialization.</returns>
        Task InitializeAsync();

        /// <summary>
        /// Loads the bundle into a JavaScript bridge.
        /// </summary>
        /// <param name="bridge">The JavaScript bridge.</param>
        void LoadScript(IReactBridge bridge);
    }
}
