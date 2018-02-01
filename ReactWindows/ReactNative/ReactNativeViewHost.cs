using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ReactNative
{
    /// <summary>
    /// Simple class that holds the <see cref="ReactNativeAppHost"/> used in the multi window applications. 
    /// </summary>
    public abstract class ReactNativeViewHost : IAsyncDisposable
    {
        /// <summary>
        /// Instantiates the <see cref="ReactNativeViewHost"/>.
        /// </summary>
        public ReactNativeViewHost(ReactNativeAppHost reactNativeAppHost)
        {
            ReactNativeAppHost = reactNativeAppHost;
        }

        /// <summary>
        /// The <see cref="ReactNativeAppHost"/> associated with this view.
        /// </summary>
        public ReactNativeAppHost ReactNativeAppHost { get; private set; }

        /// <summary>
        /// The root view managed by the view host.
        /// </summary>
        public ReactRootView RootView { get; private set; }

        /// <summary>
        /// Called when the view is first initialized.
        /// </summary>
        public ReactRootView OnCreate()
        {
            return OnCreate(null);
        }

        /// <summary>
        /// Called when the view is first initialized.
        /// </summary>
        /// <param name="initialProps">The initial props.</param>
        public ReactRootView OnCreate(JObject initialProps)
        {
            RootView = CreateRootView();
            RootView.OnCreate(this);
            RootView.StartReactApplication(
                ReactNativeAppHost.ReactInstanceManager,
                MainComponentName,
                initialProps);

             return RootView;
        }
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

        /// <summary>
        /// Called before the application shuts down.
        /// </summary>
        public async Task DisposeAsync()
        {
            await RootView.StopReactApplicationAsync();
        }
    }
}
