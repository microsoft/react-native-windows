using Newtonsoft.Json.Linq;
using System;

namespace ReactNative
{
    /// <summary>
    /// An application delegate for managing the lifecycle events in React Native.
    /// </summary>
    public static class ReactNativeHostExtensions
    {
        /// <summary>
        /// Called when the application is first initialized.
        /// </summary>
        /// <param name="host">The React Native host.</param>
        public static ReactRootView OnCreate(this ReactNativeHost host)
        {
            return OnCreate(host, null);
        }

        /// <summary>
        /// Called when the application is first initialized.
        /// </summary>
        /// <param name="host">The React Native host.</param>
        /// <param name="initialProps">The initial props.</param>
        public static ReactRootView OnCreate(this ReactNativeHost host, JObject initialProps)
        {
            var rootView = host.CreateRootView();
            rootView.OnCreate(host);
            rootView.StartReactApplication(
                host.ReactInstanceManager, 
                host.MainComponentName,
                initialProps);
            return rootView;
        }
    }
}
