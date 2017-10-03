namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Types of <see cref="IActionQueue"/>.
    /// </summary>
    public enum ActionQueueKind
    {
        /// <summary>
        /// Dispatcher queue type.
        /// </summary>
        Dispatcher,
        
        /// <summary>
        /// New thread queue type.
        /// </summary>
        NewThread,

        /// <summary>
        /// Task pool queue type.
        /// </summary>
        TaskPool,

        /// <summary>
        /// Layout queue type.
        /// </summary>
        Layout,
    }

}
