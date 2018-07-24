using System;
using Windows.UI.Xaml.Controls;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using Newtonsoft.Json.Linq;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// A view manager responsible for rendering a CalendarDatePicker.
    /// </summary>
    public class ReactCalendarDatePickerManager : BaseViewManager<CalendarDatePicker, ReactCalendarDatePickerShadowNode>
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "CalendarDatePickerWindows";
            }
        }

        /// <summary>
        /// Sets the value of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The new value.</param>
        [ReactProp("date")]
        public void SetDate(CalendarDatePicker view, long? date)
        {
            view.Date = date.HasValue ? DateTimeOffset.FromUnixTimeMilliseconds(date.Value) : (DateTimeOffset?)null;
        }


        /// <summary>
        /// Sets the maximum allowed date of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The value to set as maximum.</param>
        [ReactProp("maxDate")]
        public void SetMaxDate(CalendarDatePicker view, long? date) => view.MaxDate = date.HasValue ? DateTimeOffset.FromUnixTimeMilliseconds(date.Value) : DateTimeOffset.MaxValue;

        /// <summary>
        /// Sets the minimum allowed date of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The value to set as minimum.</param>
        [ReactProp("minDate")]
        public void SetMinDate(CalendarDatePicker view, long? date) => view.MinDate = date.HasValue ? DateTimeOffset.FromUnixTimeMilliseconds(date.Value) : DateTimeOffset.MinValue;

        /// <summary>
        /// Sets the placeholder text for the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="placeholderText">The value of the placeholder.</param>
        [ReactProp("placeholderText")]
        public void SetPlaceholderText(CalendarDatePicker view, string placeholderText)
        {
            view.PlaceholderText = placeholderText;
        }

        /// <summary>
        /// Sets the <see cref="Windows.Globalization.CalendarIdentifiers"/> value for the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="identifier">The identifier, which must be one of the values defined in <see cref="Windows.Globalization.CalendarIdentifiers"/></param>
        [ReactProp("calendarIdentifier")]
        public void SetCalendarIdentifier(CalendarDatePicker view, string identifier)
        {
            if (identifier.Length > 0)
            {
                view.CalendarIdentifier = identifier;
            }
        }

        /// <summary>
        /// Sets the first day of the week for the picker.
        /// </summary>
        /// <param name="view">The picker instance.</param>
        /// <param name="day">The day.</param>
        [ReactProp("firstDayOfWeek")]
        public void SetFirstDayOfWeek(CalendarDatePicker view, int day)
        {
            view.FirstDayOfWeek = (Windows.Globalization.DayOfWeek)day;
        }

        /// <summary>
        /// This method returns the <see cref="ReactCalendarDatePickerShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactCalendarDatePickerShadowNode CreateShadowNodeInstance()
        {
            return new ReactCalendarDatePickerShadowNode();
        }

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(CalendarDatePicker root, object extraData)
        {
        }

        /// <summary>
        /// Unbind event handlers
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, CalendarDatePicker view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.DateChanged -= OnDateChanged;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="CalendarDatePicker"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override CalendarDatePicker CreateViewInstance(ThemedReactContext reactContext)
        {
            CalendarDatePicker picker = new CalendarDatePicker();
            return picker;
        }

        /// <summary>
        /// Binds the <see cref="CalendarDatePicker.DateChanged"/> event to a handler
        /// </summary>
        /// <param name="reactContext">The react context.</param>
        /// <param name="view">The view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, CalendarDatePicker view)
        {
            base.AddEventEmitters(reactContext, view);
            view.DateChanged += OnDateChanged;
        }

        /// <summary>
        /// Date changed update handler
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event.</param>
        private void OnDateChanged(object sender, CalendarDatePickerDateChangedEventArgs e)
        {
            if (!e.NewDate.HasValue)
            {
                return;
            }

            var datePicker = (CalendarDatePicker)sender;
            DateTimeOffset newDate = new DateTimeOffset(e.NewDate.Value.DateTime);
            
            datePicker.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new ReactCalendarPickerEvent(datePicker.GetTag(), newDate.ToUnixTimeMilliseconds()));
        }

        /// <summary>
        /// A CalendarDatePicker-specific event to communicate with JavaScript
        /// </summary>
        class ReactCalendarPickerEvent : Event
        {
            private readonly long? _date;
            /// <summary>
            /// Creates an instance of the event
            /// </summary>
            /// <param name="viewTag">The viewtag of the instantiating view.</param>
            /// <param name="date">Date to include in the event payload.</param>
            public ReactCalendarPickerEvent(int viewTag, long? date) :
                base(viewTag)
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
                    {"date", _date }
                };
                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
            }
        }
    }
}
