using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;

namespace BV.LinearGradient
{
    /// <summary>
    /// Package defining core framework modules (e.g., <see cref="UIManagerModule"/>). 
    /// It should be used for modules that require special integration with
    /// other framework parts (e.g., with the list of packages to load view
    /// managers from).
    /// </summary>
    public class LinearGradientPackage : IReactPackage
    {
        /// <summary>
        /// Creates the list of native modules to register with the react
        /// instance. 
        /// </summary>
        /// <param name="reactContext">The react application context.</param>
        /// <returns>The list of native modules.</returns>
        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            return new List<INativeModule>(0);
        }

        /// <summary>
        /// Creates the list of JavaScript modules to register with the 
        /// react instance. 
        /// </summary>
        /// <returns>The list of JavaScript modules.</returns>
        public IReadOnlyList<Type> CreateJavaScriptModulesConfig()
        {
            return new List<Type>(0);
        }

        /// <summary>
        /// Creates the list of view managers that should be registered with
        /// the <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="reactContext">The react application context.</param>
        /// <returns>The list of view managers.</returns>
        public IReadOnlyList<IViewManager> CreateViewManagers(
            ReactContext reactContext)
        {
            return new List<IViewManager>
            {
                new LinearGradientViewManager(),
            };
        }
    }
}
