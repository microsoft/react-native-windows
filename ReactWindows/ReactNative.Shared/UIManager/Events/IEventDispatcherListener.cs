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
        void OnEventDispatch(Event @event);
    }
}
