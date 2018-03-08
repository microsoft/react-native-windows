// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

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
