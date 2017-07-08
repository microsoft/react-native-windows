using System;
using Windows.UI.Xaml.Controls;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using Newtonsoft.Json.Linq;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// A view manager responsible for rendering a DatePicker
    /// </summary>
    public class ReactTimePickerManager : BaseViewManager<Windows.UI.Xaml.Controls.TimePicker, ReactTimePickerShadowNode>
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTTimePicker";
            }
        }
        // TODO: Review grammar
        /// <summary>
        /// Sets the value of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The new value.</param>
        [ReactProp("date")]
        public void SetDate(Windows.UI.Xaml.Controls.TimePicker view, DateTime? date)
        {
            if (date.HasValue)
            {
                view.Time = date.Value.TimeOfDay;
            }
        }

        /// <summary>
        /// This method returns the <see cref="ReactTimePickerShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactTimePickerShadowNode CreateShadowNodeInstance()
        {
            return new ReactTimePickerShadowNode();
        }

        /// <summary>
        /// Unbind event handlers
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.TimePicker view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.TimeChanged -= OnTimeChanged;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Windows.UI.Xaml.Controls.TimePicker"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Windows.UI.Xaml.Controls.TimePicker CreateViewInstance(ThemedReactContext reactContext)
        {
            Windows.UI.Xaml.Controls.TimePicker picker = new Windows.UI.Xaml.Controls.TimePicker();
            return picker;
        }

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(Windows.UI.Xaml.Controls.TimePicker root, object extraData)
        {
        }

        /// <summary>
        /// Binds the <see cref="Windows.UI.Xaml.Controls.TimePicker.TimeChanged"/> event to a handler.
        /// The event dispatches a <see cref="DateTime" /> instance with the time information and the date set to the current day
        /// </summary>
        /// <param name="reactContext">The react context</param>
        /// <param name="view">The view instance</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.TimePicker view)
        {
            base.AddEventEmitters(reactContext, view);
            view.TimeChanged += OnTimeChanged;
        }

        private void OnTimeChanged(object sender, TimePickerValueChangedEventArgs e)
        {
            var view = (Windows.UI.Xaml.Controls.TimePicker)sender;
            TimeSpan time = e.NewTime;
            DateTime now = DateTime.UtcNow;
            DateTime dateTime = new DateTime(now.Year, now.Month, now.Day, time.Hours, time.Minutes, time.Seconds);

            view.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new ReactTimePickerEvent(view.GetTag(), dateTime));
        }

        /// <summary>
        /// A TimePicker-specific event to communicate with JavaScript
        /// </summary>
        class ReactTimePickerEvent : Event
        {
            private readonly DateTime _date;
            /// <summary>
            /// Creates an instance of the event
            /// </summary>
            /// <param name="viewTag">The viewtag of the instantiating view.</param>
            /// <param name="date">Date to include in the event payload.</param>
            public ReactTimePickerEvent(int viewTag, DateTime date) :
                base(viewTag, TimeSpan.FromTicks(Environment.TickCount))
            {
                _date = date;
            }

            /// <summary>
            /// Internal name of the event
            /// </summary>
            public override string EventName
            {
                get
                {
                    return "topChange";
                }
            }

            /// <summary>
            /// Dispatches the event through the JavaScript bridge
            /// </summary>
            /// <param name="eventEmitter"></param>
            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                JObject eventData = new JObject
                {
                    {"date", _date },
                };
                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
            }
        }
    }
}
