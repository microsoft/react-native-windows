using ReactNative.UIManager.Events;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface responsible for knowing how to get and set
    /// an instance of EventDispatcher
    /// </summary>
    public interface IEventEmitter
    {
        /// <summary>
        /// The instance of the EventDispatcher relevant to the implementer's context
        /// </summary>
        IEventDispatcher EventDispatcher { get; set; }
    }
}