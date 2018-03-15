// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.PlatformServices
{
    /// <summary>
    /// Interface for resolving platform-specific services.
    /// </summary>
    public interface IServiceProvider
    {
        /// <summary>
        /// Gets the platform-specific service instance.
        /// </summary>
        /// <typeparam name="T">Type of service.</typeparam>
        /// <returns>The platform-specific instance.</returns>
        T GetService<T>() where T : class;
    }
}
