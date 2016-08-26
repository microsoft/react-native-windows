namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Types of <see cref="IMessageQueueThread"/>.
    /// </summary>
    public enum MessageQueueThreadKind
    {
        /// <summary>
        /// Dispatcher thread type.
        /// </summary>
        DispatcherThread,

        /// <summary>
        /// Layout thread type.
        /// </summary>
        LayoutThread,
        
        /// <summary>
        /// Single background thread type.
        /// </summary>
        BackgroundSingleThread,

        /// <summary>
        /// Any background thread type.
        /// </summary>
        BackgroundAnyThread,
    }

}
