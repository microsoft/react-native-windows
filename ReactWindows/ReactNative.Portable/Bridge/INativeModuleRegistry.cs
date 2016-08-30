using System.Collections.Generic;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ReactNative.Bridge
{
    public interface INativeModuleRegistry
    {
        /// <summary>
        /// The set of native modules exposed.
        /// </summary>
        IEnumerable<INativeModule> Modules { get; }

        /// <summary>
        /// Gets a module instance of a specific type.
        /// </summary>
        /// <typeparam name="T">Type of module instance.</typeparam>
        /// <returns>The module instance.</returns>
        T GetModule<T>() where T : INativeModule;

        /// <summary>
        /// Triggers the batch completion event for all modules.
        /// </summary>
        void OnBatchComplete();

        /// <summary>
        /// Hook to notify modules that the <see cref="IReactInstance"/> has
        /// been initialized.
        /// </summary>
        void NotifyReactInstanceInitialize();

        /// <summary>
        /// Hook to notify modules that the <see cref="IReactInstance"/> has
        /// been disposed.
        /// </summary>
        void NotifyReactInstanceDispose();

        /// <summary>
        /// Write the module descriptions to the given <see cref="JsonWriter"/>.
        /// </summary>
        /// <param name="writer">The JSON writer.</param>
        void WriteModuleDescriptions(JsonWriter writer);

        /// <summary>
        /// Invoke a method on a native module.
        /// </summary>
        /// <param name="reactInstance">The React instance.</param>
        /// <param name="moduleId">The module ID.</param>
        /// <param name="methodId">The method ID.</param>
        /// <param name="parameters">The parameters.</param>
        void Invoke(
            IReactInstance reactInstance,
            int moduleId,
            int methodId,
            JArray parameters);
    }
}
