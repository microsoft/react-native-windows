namespace ReactNative.UIManager.Events
{
    /// <summary>
    /// Interface used to intercept events dispatched by <see cref="EventDispatcher"/>.
    /// </summary>
    public interface IEventDispatcherListener
    {
        /// <summary>
        /// Called on every time an event is dispatched using <see cref="EventDispatcher.DispatchEvent(Event)"/>. 
        /// Will be called from the same thread that the event is being 
        /// dispatched from.
        /// </summary>
        /// <param name="event">Event that was dispatched.</param>
        /// <returns>
        /// If the event was handled. If true the event won't be sent to 
        /// JavaScript.
        /// </returns>
        bool OnEventDispatch(Event @event);
    }
}
