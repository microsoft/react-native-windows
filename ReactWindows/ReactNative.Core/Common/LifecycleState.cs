namespace ReactNative.Common
{
    /// <summary>
    /// An enumeration to signify the current lifecycle state for a 
    /// <see cref="ReactInstanceManager"/>.
    /// </summary>
    public enum LifecycleState
    {
        /// <summary>
        /// Lifecycle state before an application is initially created.
        /// </summary>
        BeforeCreate,

        /// <summary>
        /// Lifecycle state before an application is resumed.
        /// </summary>
        BeforeResume,

        /// <summary>
        /// Lifecycle state of a resumed application.
        /// </summary>
        Resumed,

        /// <summary>
        /// Lifecycle state when the application is in the background.
        /// </summary>
        Background,
    }
}
