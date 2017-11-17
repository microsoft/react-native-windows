using ReactNative.UIManager;
using System.Threading;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.Views.TextInput
{
    class ReactTextBox : TextBox
    {
        private int _eventCount;
        private bool _selectionChangedSubscribed;

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

        public bool OnSelectionChange
        {
            get
            {
                return _selectionChangedSubscribed;
            }
            set
            {
                if (value != _selectionChangedSubscribed)
                {
                    _selectionChangedSubscribed = value;
                    if (_selectionChangedSubscribed)
                    {
                        this.SelectionChanged += OnSelectionChanged;
                    }
                    else
                    {
                        this.SelectionChanged -= OnSelectionChanged;
                    }
                }
            }
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
            this.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextChangedEvent(
                        this.GetTag(),
                        Text,
                        e.NewSize.Width,
                        e.NewSize.Height,
                        IncrementEventCount()));
        }

        private void OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            var start = this.SelectionStart;
            var length = this.SelectionLength;
            this.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextInputSelectionEvent(
                        this.GetTag(),
                        start,
                        start + length));
        }
    }
}
