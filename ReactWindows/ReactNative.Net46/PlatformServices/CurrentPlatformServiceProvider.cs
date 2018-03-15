namespace ReactNative.PlatformServices
{
    /// <summary>
    /// Service provider implementation for WPF.
    /// </summary>
    public class CurrentPlatformServiceProvider : IServiceProvider
    {
        /// <summary>
        /// Gets the platform-specific service instance.
        /// </summary>
        /// <typeparam name="T">Type of service.</typeparam>
        /// <returns>The platform-specific instance.</returns>
        public T GetService<T>() where T : class
        {
            return null;
        }
    }
}
