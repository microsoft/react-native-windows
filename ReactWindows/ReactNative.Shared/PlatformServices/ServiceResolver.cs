// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.PlatformServices
{
    /// <summary>
    /// Singleton for platform-specific service provider.
    /// </summary>
    public static class ServiceResolver
    {
        private static IServiceProvider s_current = new CurrentPlatformServiceProvider();

        /// <summary>
        /// The current platform service provider.
        /// </summary>
        public static IServiceProvider Current
        {
            get
            {
                return s_current;
            }
            set
            {
                if (value == null)
                    throw new ArgumentNullException(nameof(value));

                s_current = value;
            }
        }

        /// <summary>
        /// Gets the platform-specific service instance.
        /// </summary>
        /// <typeparam name="T">Type of service.</typeparam>
        /// <returns>The platform-specific instance.</returns>
        public static T GetService<T>() where T : class
        {
            return s_current.GetService<T>();
        }
    }
}
