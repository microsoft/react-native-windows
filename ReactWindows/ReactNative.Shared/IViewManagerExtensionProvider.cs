using ReactNative.Bridge;
using ReactNative.UIManager;
using System.Collections.Generic;

namespace ReactNative
{
    /// <summary>
    /// Interface for exposing native view manager extensions.
    /// </summary>
    /// <remarks>
    /// To add <see cref="IViewManagerExtension"/> instances to your application,
    /// extend your <see cref="Modules.Core.IReactPackage" /> implementation with this
    /// interface, e.g.:
    /// <code>
    /// class ReactPackage : IReactPackage, IViewManagerExtensionProvider
    /// {
    ///     /* ... IReactPackage implementation ... */
    ///
    ///     public IReadOnlyList&lt;IViewManagerExtension&gt; CreateViewManagerExtensions(ReactContext reactContext) =>
    ///         new List&lt;IViewManagerExtension&gt;
    ///         {
    ///             new MyViewManagerExtension(),
    ///         };
    /// }
    /// </code>
    /// </remarks>
    public interface IViewManagerExtensionProvider
    {
        /// <summary>
        /// Creates the list of view manager extensions that should be
        /// registered with the <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The list of view managers.</returns>
        IReadOnlyList<IViewManagerExtension> CreateViewManagerExtensions(ReactContext reactContext);
    }
}
