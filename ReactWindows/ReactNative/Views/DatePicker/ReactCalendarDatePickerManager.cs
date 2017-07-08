using System;
using Windows.UI.Xaml.Controls;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using Newtonsoft.Json.Linq;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// A view manager responsible for rendering a CalendarDatePicker
    /// </summary>
    public class ReactCalendarDatePickerManager : BaseViewManager<Windows.UI.Xaml.Controls.CalendarDatePicker, ReactCalendarDatePickerShadowNode>
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTCalendarDatePicker";
            }
        }

        /// <summary>
        /// Sets the value of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The new value.</param>
        [ReactProp("date")]
        public void SetDate(Windows.UI.Xaml.Controls.CalendarDatePicker view, DateTime date)
        {
            view.Date = date;
            /*view.CalendarIdentifier;
            view.DayOfWeekFormat;
            view.FirstDayOfWeek;*/
            
        }

        /// <summary>
        /// Sets the maximum allowed date of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The value to set as maximum.</param>
        [ReactProp("maxDate")]
        public void SetMaxDate(Windows.UI.Xaml.Controls.CalendarDatePicker view, DateTime date)
        {
            view.MaxDate = date;
        }

        /// <summary>
        /// Sets the minimum allowed date of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The value to set as minimum.</param>
        [ReactProp("minDate")]
        public void SetMinDate(Windows.UI.Xaml.Controls.CalendarDatePicker view, DateTime date)
        {
            view.MinDate = date;
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
        public override void UpdateExtraData(Windows.UI.Xaml.Controls.CalendarDatePicker root, object extraData)
        {
        }

        /// <summary>
        /// Unbind event handlers
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.CalendarDatePicker view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.DateChanged -= OnDateChanged;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Windows.UI.Xaml.Controls.CalendarDatePicker"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Windows.UI.Xaml.Controls.CalendarDatePicker CreateViewInstance(ThemedReactContext reactContext)
        {
            Windows.UI.Xaml.Controls.CalendarDatePicker picker = new Windows.UI.Xaml.Controls.CalendarDatePicker();
            return picker;
        }

        /// <summary>
        /// Binds the <see cref="Windows.UI.Xaml.Controls.CalendarDatePicker.DateChanged"/> event to a handler
        /// </summary>
        /// <param name="reactContext">The react context</param>
        /// <param name="view">The view instance</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.CalendarDatePicker view)
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
            var datePicker = (Windows.UI.Xaml.Controls.CalendarDatePicker)sender;
            if (!e.NewDate.HasValue)
            {
                return;
            }
            DateTime newDate = e.NewDate.Value.DateTime;

            datePicker.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new ReactCalendarDatePickerEvent(datePicker.GetTag(), newDate));
        }

        /// <summary>
        /// A CalendarDatePicker-specific event to communicate with JavaScript
        /// </summary>
        class ReactCalendarDatePickerEvent : Event
        {
            private readonly DateTime _date;
            /// <summary>
            /// Creates an instance of the event
            /// </summary>
            /// <param name="viewTag">The viewtag of the instantiating view.</param>
            /// <param name="date">Date to include in the event payload.</param>
            public ReactCalendarDatePickerEvent(int viewTag, DateTime date) :
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
