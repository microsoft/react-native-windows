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
    public class ReactDateTimePickerManager : BaseViewManager<StackPanel, ReactDateTimePickerShadowNode>
    {
        private StackPanel panel;
        private Windows.UI.Xaml.Controls.DatePicker datePicker;
        private TimePicker timePicker;
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTDateTimePicker";
            }
        }

        /// <summary>
        /// Sets the value of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The new value.</param>
        [ReactProp("date")]
        public void SetDate(StackPanel view, DateTime? date)
        {
            if (date.HasValue)
            {
                datePicker.Date = date.Value;
                timePicker.Time = date.Value.TimeOfDay;
            }
        }

        /// <summary>
        /// Sets the maximum allowed year of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The value to set as maximum.</param>
        [ReactProp("maxYear")]
        public void SetMaxYear(StackPanel view, DateTime? date)
        {
            if (date.HasValue)
            {
                datePicker.MaxYear = date.Value;
            }
        }

        /// <summary>
        /// Sets the minimum allowed year of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The value to set as minimum.</param>
        [ReactProp("minYear")]
        public void SetMinYear(StackPanel view, DateTime? date)
        {
            if (date.HasValue)
            {
                datePicker.MinYear = date.Value;
            }
        }

        /// <summary>
        /// This method returns the <see cref="ReactDatePickerShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactDateTimePickerShadowNode CreateShadowNodeInstance()
        {
            return new ReactDateTimePickerShadowNode();
        }

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(StackPanel root, object extraData)
        {
        }

        /// <summary>
        /// Unbind event handlers
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, StackPanel view)
        {
            base.OnDropViewInstance(reactContext, view);
            datePicker.DateChanged -= OnDateChanged;
            timePicker.TimeChanged -= OnTimeChanged;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="StackPanel"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override StackPanel CreateViewInstance(ThemedReactContext reactContext)
        {
            panel = new StackPanel();
            panel.Orientation = Orientation.Vertical;
            datePicker = new Windows.UI.Xaml.Controls.DatePicker();
            timePicker = new TimePicker();

            timePicker.HorizontalAlignment = Windows.UI.Xaml.HorizontalAlignment.Stretch;
            datePicker.HorizontalAlignment = Windows.UI.Xaml.HorizontalAlignment.Stretch;

            panel.Children.Add(timePicker);
            panel.Children.Add(datePicker);

            return panel;
        }

        /// <summary>
        /// Binds the internal <see cref="Windows.UI.Xaml.Controls.DatePicker.DateChanged"/> and <see cref="Windows.UI.Xaml.Controls.TimePicker.TimeChanged"/> events to a handler
        /// </summary>
        /// <param name="reactContext">The react context</param>
        /// <param name="view">The view instance</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, StackPanel view)
        {
            base.AddEventEmitters(reactContext, view);
            datePicker.DateChanged += OnDateChanged;
            timePicker.TimeChanged += OnTimeChanged;
        }

        /// <summary>
        /// Date changed update handler
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event.</param>
        private void OnDateChanged(object sender, DatePickerValueChangedEventArgs e)
        {
            DateTime newDate = e.NewDate.DateTime;

            DateTime combinedDateTime = new DateTime(newDate.Year, newDate.Month, newDate.Day, timePicker.Time.Hours, timePicker.Time.Minutes, timePicker.Time.Seconds);

            panel.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new ReactDateTimePickerEvent(panel.GetTag(), combinedDateTime));
        }

        /// <summary>
        /// Time changed update handler. Combines the new time with the date of the <see cref="datePicker" /> and emits a dateChanged event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnTimeChanged(object sender, TimePickerValueChangedEventArgs e)
        {
            TimeSpan newTime = e.NewTime;
            DateTime combinedDateTime = new DateTime(datePicker.Date.Year, datePicker.Date.Month, datePicker.Date.Day, newTime.Hours, newTime.Minutes, newTime.Days);

            panel.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new ReactDateTimePickerEvent(panel.GetTag(), combinedDateTime));
        }

        /// <summary>
        /// A DatePicker-specific event to communicate with JavaScript
        /// </summary>
        class ReactDateTimePickerEvent : Event
        {
            private readonly DateTime _date;
            /// <summary>
            /// Creates an instance of the event
            /// </summary>
            /// <param name="viewTag">The viewtag of the instantiating view.</param>
            /// <param name="date">Date to include in the event payload.</param>
            public ReactDateTimePickerEvent(int viewTag, DateTime date) :
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
