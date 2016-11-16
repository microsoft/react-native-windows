using ReactNative.UIManager;
using System.Threading;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using ReactNative.UIManager.Events;
using System;

namespace ReactNative.Views.TextInput
{
    class ReactTextBox : TextBox, IEventEmitter
    {
        private int _eventCount;

        public ReactTextBox()
        {
            SizeChanged += OnSizeChanged;
        }

        public int CurrentEventCount
        {
            get
            {
                return _eventCount;
            }
        }

        public bool ClearTextOnFocus
        {
            get;
            set;
        }

        public bool SelectTextOnFocus
        {
            get;
            set;
        }

        public int IncrementEventCount()
        {
            return Interlocked.Increment(ref _eventCount);
        }

        protected override void OnGotFocus(RoutedEventArgs e)
        {
            base.OnGotFocus(e);
            if (ClearTextOnFocus)
            {
                Text = "";
            }

            if (SelectTextOnFocus)
            {
                SelectionStart = 0;
                SelectionLength = Text.Length;
            }
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            EventDispatcher
                .DispatchEvent(
                    new ReactTextChangedEvent(
                        this.GetTag(),
                        Text,
                        e.NewSize.Width,
                        e.NewSize.Height,
                        IncrementEventCount()));
        }

        #region IEventEmitter

        public EventDispatcher EventDispatcher { get; set; }

        public void AssertEventDispatcher()
        {
            if (EventDispatcher == null)
            {
                throw new InvalidOperationException("Event Dispatcher is null");
            }
        }

        #endregion
    }
}
