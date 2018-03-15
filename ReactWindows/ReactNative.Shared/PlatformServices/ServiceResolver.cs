namespace ReactNative.PlatformServices
{
    /// <summary>
    /// Singleton for platform-specific service provider.
    /// </summary>
    public static class ServiceResolver
    {
        /// <summary>
        /// The current platform service provider.
        /// </summary>
        public static IServiceProvider Instance { get; } = new CurrentPlatformServiceProvider();
    }
}
