using ReactNative.UIManager.Events;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface responsible for knowing how to dispatch and listen for UIManager events...
    /// </summary>
    public interface IEventDispatcher
    {
        /// <summary>
        /// Sends the given <see cref="Event"/> to JavaScript, coalescing
        /// events if JavaScript is backed up.
        /// </summary>
        /// <param name="event">The event.</param>
        void DispatchEvent(Event @event);

        /// <summary>
        /// Adds a listener to this <see cref="EventDispatcher"/>. 
        /// </summary>
        /// <param name="listener">The listener.</param>
        void AddListener(IEventDispatcherListener listener);

        /// <summary>
        /// Removes a listener from this <see cref="EventDispatcher"/>. 
        /// </summary>
        /// <param name="listener">The listener.</param>
        void RemoveListener(IEventDispatcherListener listener);
    }
}