using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ReactNative
{
    /// <summary>
    /// Simple class that is responsible for the creation of one <see cref="ReactRootView"/>. 
    /// </summary>
    public abstract class ReactNativeHost : ReactNativeAppHost
    {
        /// <summary>
        /// The main component name.
        /// </summary>
        public abstract string MainComponentName { get; }


        /// <summary>
        /// Creates a new root view.
        /// </summary>
        /// <returns>The root view.</returns>
        public virtual ReactRootView CreateRootView()
        {
            return new ReactRootView();
        }
    }
}
