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

        private Button DeleteButton
        {
            get;
            set;
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

        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            if (DeleteButton == null)
            {
                DeleteButton = (Button)GetTemplateChild("DeleteButton");
                if (DeleteButtonVisibilityToken != null)
                {
                    DeleteButton.UnregisterPropertyChangedCallback(Button.VisibilityProperty, DeleteButtonVisibilityToken ?? 0);
                }
                DeleteButtonVisibilityToken = DeleteButton.RegisterPropertyChangedCallback(Button.VisibilityProperty, (DependencyObject d, DependencyProperty dp) => UpdateDeleteButtonVisibility());
            }
        }

        private void UpdateDeleteButtonVisibility()
        {
            if (DeleteButton != null)
            {
                switch (ClearButtonMode)
                {
                    case ClearButtonModeType.Default:
                        break;
                    case ClearButtonModeType.Never:
                        DeleteButton.Visibility = Visibility.Collapsed;
                        break;
                    case ClearButtonModeType.Always:
                        DeleteButton.Visibility = Visibility.Visible;
                        break;
                    case ClearButtonModeType.WhileEditing:
                        DeleteButton.Visibility = IsBeingEdited() ? Visibility.Visible : Visibility.Collapsed;
                        break;
                    case ClearButtonModeType.UnlessEditing:
                        DeleteButton.Visibility = IsBeingEdited() ? Visibility.Collapsed : Visibility.Visible;
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
            return !(FocusState == FocusState.Unfocused);
        }
    }
}
