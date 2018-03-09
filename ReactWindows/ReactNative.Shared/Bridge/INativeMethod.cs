using Newtonsoft.Json.Linq;

namespace ReactNative.Bridge
{
    /// <summary>
    /// An interface representing native methods.
    /// </summary>
    public interface INativeMethod
    {
        /// <summary>
        /// The type of method.
        /// </summary>
        string Type { get; }

        /// <summary>
        /// Invoke the native method.
        /// </summary>
        /// <param name="reactInstance">The React instance.</param>
        /// <param name="arguments">The arguments.</param>
        void Invoke(IReactInstance reactInstance, JArray arguments);
    }
}
