// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.UIManager;
using System;
using System.Threading;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
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
        private bool _sizeChangedSubscribed;
        private ClearButtonModeType _clearButtonMode = ClearButtonModeType.Default;
        private Button _deleteButton;

#if WINDOWS_UWP
        public event KeyEventHandler PreKeyDown;
#endif
        public ClearButtonModeType ClearButtonMode
        {
            get
            {
                return _clearButtonMode;
            }
            set
            {
                if (_clearButtonMode != value)
                {
                    _clearButtonMode = value;
                    UpdateDeleteButtonVisibility();
                }
            }
        }

        private long? DeleteButtonVisibilityToken
        {
            get;
            set;
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
                        SelectionChanged += OnSelectionChanged;
                    }
                    else
                    {
                        SelectionChanged -= OnSelectionChanged;
                    }
                }
            }
        }

        public bool OnContentSizeChange
        {
            get
            {
                return _sizeChangedSubscribed;
            }
            set
            {
                if (value != _sizeChangedSubscribed)
                {
                    _sizeChangedSubscribed = value;
                    if (_sizeChangedSubscribed)
                    {
                        SizeChanged += OnSizeChanged;
                    }
                    else
                    {
                        SizeChanged -= OnSizeChanged;
                    }
                }
            }
        }

        public bool AutoGrow
        {
            get;
            set;
        }

        public bool DimensionsUpdated
        {
            get;
            set;
        }

        public int IncrementEventCount()
        {
            return Interlocked.Increment(ref _eventCount);
        }

#if WINDOWS_UWP
        protected override void OnApplyTemplate()
#else
        public override void OnApplyTemplate()
#endif
        {
            base.OnApplyTemplate();

            if (_deleteButton != null)
            {
                if (DeleteButtonVisibilityToken.HasValue)
                {
#if WINDOWS_UWP
                    _deleteButton.UnregisterPropertyChangedCallback(Button.VisibilityProperty, (long)DeleteButtonVisibilityToken);
#endif
                }
            }

            _deleteButton = (Button)GetTemplateChild("DeleteButton");
#if WINDOWS_UWP
            DeleteButtonVisibilityToken = _deleteButton.RegisterPropertyChangedCallback(Button.VisibilityProperty, (DependencyObject d, DependencyProperty dp) => UpdateDeleteButtonVisibility());
#endif
            TextChanged += OnTextChanged;
        }

        private void UpdateDeleteButtonVisibility()
        {
            if (_deleteButton != null)
            {
                switch (ClearButtonMode)
                {
                    case ClearButtonModeType.Default:
                        break;
                    case ClearButtonModeType.Never:
                        _deleteButton.Visibility = Visibility.Collapsed;
                        break;
                    case ClearButtonModeType.Always:
                        _deleteButton.Visibility = Visibility.Visible;
                        break;
                    case ClearButtonModeType.WhileEditing:
                        _deleteButton.Visibility = IsBeingEdited() ? Visibility.Visible : Visibility.Collapsed;
                        break;
                    case ClearButtonModeType.UnlessEditing:
                        _deleteButton.Visibility = UnlessBeingEdited() ? Visibility.Visible : Visibility.Collapsed;
                        break;
                    default:
                        throw new NotSupportedException($"'{ClearButtonMode}' is not a mode supported by ClearButtonMode property.");
                }
            }
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

            UpdateDeleteButtonVisibility();
        }

        protected override void OnLostFocus(RoutedEventArgs e)
        {
            base.OnLostFocus(e);
            UpdateDeleteButtonVisibility();
        }

        private void OnTextChanged(object sender, TextChangedEventArgs e)
        {
            UpdateDeleteButtonVisibility();
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (DimensionsUpdated)
            {
                DimensionsUpdated = false;
                return;
            }

            this.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextInputContentSizeChangedEvent(
                        this.GetTag(),
                        e.NewSize.Width,
                        e.NewSize.Height));
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

        private bool IsBeingEdited()
        {
#if WINDOWS_UWP
            return FocusState != FocusState.Unfocused && !string.IsNullOrEmpty(Text);
#else
            return this.IsKeyboardFocused && !string.IsNullOrEmpty(Text);
#endif
        }

        private bool UnlessBeingEdited()
        {
#if WINDOWS_UWP
            return FocusState == FocusState.Unfocused && !string.IsNullOrEmpty(Text);
#else
            return !this.IsKeyboardFocused && !string.IsNullOrEmpty(Text);
#endif
        }
    }
}
