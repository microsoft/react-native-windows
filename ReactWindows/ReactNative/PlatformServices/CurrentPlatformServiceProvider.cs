// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Tracing;
using System;

namespace ReactNative.PlatformServices
{
    /// <summary>
    /// Service provider implementation for UWP.
    /// </summary>
    public class CurrentPlatformServiceProvider : IServiceProvider
    {
        private readonly Lazy<ITracer> _tracer = new Lazy<ITracer>(() => new LoggingChannelTracer());

        /// <summary>
        /// Gets the platform-specific service instance.
        /// </summary>
        /// <typeparam name="T">Type of service.</typeparam>
        /// <returns>The platform-specific instance.</returns>
        public T GetService<T>() where T : class
        {
            if (typeof(T) == typeof(ITracer))
            {
                return (T)_tracer.Value;
            }

            return null;
        }
    }
}
