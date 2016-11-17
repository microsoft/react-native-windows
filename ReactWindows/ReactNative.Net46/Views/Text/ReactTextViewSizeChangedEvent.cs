using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;
using System;
using System.Windows;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// Event emitted by <see cref="System.Windows.Controls.TextBlock"/> native 
    /// view when the control's size changed.
    /// </summary>
    public class ReactTextViewSizeChangedEvent : Event
    {
        /// <summary>
        /// Instantiate a <see cref="ReactTextViewSizeChangedEvent"/>.
        /// </summary>
        /// <param name="viewTag">The view tag.</param>
        public ReactTextViewSizeChangedEvent(int viewTag, Size newSize, Size prevSize, bool heightChanged, bool widthChanged) 
            : base(viewTag, TimeSpan.FromTicks(Environment.TickCount))
        {
            NewSize = newSize;
            PrevSize = prevSize;
            HeightChanged = heightChanged;
            WidthChanged = widthChanged;
        }

        public Size NewSize { get; }
        public Size PrevSize { get; }
        public bool HeightChanged { get; }
        public bool WidthChanged { get; }

        /// <summary>
        /// The event name.
        /// </summary>
        public override string EventName
        {
            get
            {
                return "sizeChanged";
            }
        }

        /// <summary>
        /// Disabling event coalescing.
        /// </summary>
        /// <remarks>
        /// Return false if the event can never be coalesced.
        /// </remarks>
        public override bool CanCoalesce
        {
            get
            {
                return false;
            }
        }
        /// <summary>
        /// Dispatch this event to JavaScript using the given event emitter.
        /// </summary>
        /// <param name="eventEmitter">The event emitter.</param>
        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var sizeParams = new JObject
            {
                { "newWidth", NewSize.Width },
                { "newHeight", NewSize.Height },
                { "prevWidth", PrevSize.Width },
                { "prevHeight", PrevSize.Height },
                { "heightChanged", HeightChanged },
                { "widthChanged", WidthChanged },
            };

            var eventData = new JObject
            {
                { "sizeParams", sizeParams },
                { "target", ViewTag },
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
