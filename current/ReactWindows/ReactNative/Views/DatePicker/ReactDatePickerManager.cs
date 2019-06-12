using System;
using Windows.UI.Xaml.Controls;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using Newtonsoft.Json.Linq;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// A view manager responsible for rendering a DatePicker.
    /// </summary>
    public class ReactDatePickerManager : BaseViewManager<Windows.UI.Xaml.Controls.DatePicker, ReactDatePickerShadowNode>
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "DatePickerWindows";
            }
        }

        /// <summary>
        /// Sets the value of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="date">The new value.</param>
        [ReactProp("date")]
        public void SetDate(Windows.UI.Xaml.Controls.DatePicker view, long? date)
        {
            if (date.HasValue)
            {
                view.Date = DateTimeOffset.FromUnixTimeMilliseconds(date.Value);
            }
        }

        /// <summary>
        /// Sets the maximum allowed year of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="maxYearDate">The value to set as maximum.</param>
        [ReactProp("maxYear")]
        public void SetMaxYear(Windows.UI.Xaml.Controls.DatePicker view, long? maxYearDate) => view.MaxYear = maxYearDate.HasValue ? DateTimeOffset.FromUnixTimeMilliseconds(maxYearDate.Value) : DateTimeOffset.MaxValue;

        /// <summary>
        /// Sets the minimum allowed year of the picker.
        /// </summary>
        /// <param name="view">The picker view element.</param>
        /// <param name="minYearDate">The value to set as minimum.</param>
        [ReactProp("minYear")]
        public void SetMinYear(Windows.UI.Xaml.Controls.DatePicker view, long? minYearDate) => view.MinYear = minYearDate.HasValue ? DateTimeOffset.FromUnixTimeMilliseconds(minYearDate.Value) : DateTimeOffset.MinValue;

        /// <summary>
        /// This method returns the <see cref="ReactDatePickerShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactDatePickerShadowNode CreateShadowNodeInstance()
        {
            return new ReactDatePickerShadowNode();
        }

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(Windows.UI.Xaml.Controls.DatePicker root, object extraData)
        {
        }


        /// <summary>
        /// Unbind event handlers.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.DatePicker view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.DateChanged -= OnDateChanged;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Windows.UI.Xaml.Controls.DatePicker"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Windows.UI.Xaml.Controls.DatePicker CreateViewInstance(ThemedReactContext reactContext)
        {
            Windows.UI.Xaml.Controls.DatePicker picker = new Windows.UI.Xaml.Controls.DatePicker();
            return picker;
        }

        /// <summary>
        /// Binds the <see cref="Windows.UI.Xaml.Controls.DatePicker.DateChanged"/> event to a handler.
        /// </summary>
        /// <param name="reactContext">The react context</param>
        /// <param name="view">The view instance</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.DatePicker view)
        {
            base.AddEventEmitters(reactContext, view);
            view.DateChanged += OnDateChanged;
        }

        /// <summary>
        /// Date changed update handler.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event.</param>
        private void OnDateChanged(object sender, DatePickerValueChangedEventArgs e)
        {
            var datePicker = (Windows.UI.Xaml.Controls.DatePicker)sender;
            DateTimeOffset newDate = new DateTimeOffset(e.NewDate.DateTime);

            datePicker.GetReactContext().GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new ReactDatePickerEvent(datePicker.GetTag(), newDate.ToUnixTimeMilliseconds()));
        }

        /// <summary>
        /// A DatePicker-specific event to communicate with JavaScript.
        /// </summary>
        class ReactDatePickerEvent : Event
        {
            private readonly long _date;
            /// <summary>
            /// Creates an instance of the event.
            /// </summary>
            /// <param name="viewTag">The viewtag of the instantiating view.</param>
            /// <param name="date">Date to include in the event payload.</param>
            public ReactDatePickerEvent(int viewTag, long date) :
                base(viewTag)
            {
                _date = date;
            }

            /// <summary>
            /// Internal name of the event.
            /// </summary>
            public override string EventName
            {
                get
                {
                    return "topChange";
                }
            }

            /// <summary>
            /// Dispatches the event through the JavaScript bridge.
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
