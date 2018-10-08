// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if WINDOWS_UWP
using System;
using Windows.System;
using Windows.UI;
using ReactNative.Accessibility;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using ReactNative.Touch;
using ReactNative.UIManager;

#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// A native control with a single ContentDialog child.
    /// </summary>
#if WINDOWS_UWP
    public class ReactModalHostView : FrameworkElement, IAccessible
#else
    public class ReactModalHostView : UserControl
#endif
    {
        private ContentDialog _contentDialog;

        private TouchHandler _touchHandler;

        private bool _isLoaded;

        /// <summary>
        /// The current dialog content
        /// </summary>
        public DependencyObject Content
        {
            get
            {
                return _contentDialog.Content as DependencyObject;
            }
            set
            {
                if (value is FrameworkElement element)
                {
                    element.VerticalAlignment = VerticalAlignment.Top;
                    element.HorizontalAlignment = HorizontalAlignment.Stretch;
                    element.Height = double.NaN;
                    element.Width = double.NaN;

                    if (value is BorderedCanvas canvas)
                        canvas.Background = new SolidColorBrush(Colors.Transparent);

                    _touchHandler = new TouchHandler(element);
                }

                _contentDialog.Content = value;
            }
        }

        /// <summary>
        /// If the modal will render over a transparent background
        /// </summary>
        public bool Transparent { get; set; }

        /// <summary>
        /// Called when the user taps the hardware back button
        /// </summary>
        public event Action<DependencyObject> OnRequestCloseListener;

        /// <summary>
        /// Called once the modal has been shown
        /// </summary>
        public event Action<DependencyObject> OnShowListener;

        /// <summary>
        /// Instantiates the <see cref="ReactModalHostView"/>. 
        /// </summary>
        public ReactModalHostView()
        {
            Loaded += (sender, args) =>
            {
                _isLoaded = true;
                Show();
            };
        }

        /// <summary>
        /// Shows the dialog if not visible otherwise updates it's properties
        /// </summary>
        public void ShowOrUpdate()
        {
            if (this._contentDialog != null)
            {
                UpdateProperties();
                return;
            }
            
            _contentDialog = new ContentDialog
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Stretch,
                Resources =
                {
                    ["ContentDialogContentScrollViewerMargin"] = "0",
                    ["ContentDialogMaxHeight"] = double.NaN,
                    ["ContentDialogMaxWidth"] = double.NaN
                }
            };

            _contentDialog.KeyUp += (o, eventArgs) =>
            {
                if (eventArgs.Key == VirtualKey.Back)
                {
                    OnRequestCloseListener?.Invoke(Content);
                    eventArgs.Handled = true;
                }
            };

            UpdateProperties();

            if (_isLoaded)
            {
                Show();
            }
        }

        /// <summary>
        /// Closes the dialog
        /// </summary>
        public void Close()
        {
            _contentDialog.Hide();
            _touchHandler.Dispose();
        }

        private void Show()
        {
            _contentDialog.ShowAsync().GetResults();
            OnShowListener?.Invoke(Content);
        }

        private void UpdateProperties()
        {
            if (Transparent)
            {
                _contentDialog.Background = new SolidColorBrush(Colors.Transparent);
                _contentDialog.Resources["ContentDialogBorderWidth"] = "0";
            }
        }

#if WINDOWS_UWP
        /// <inheritdoc />                                              
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new DynamicAutomationPeer<ReactModalHostView>(this);
        }

        // TODO: implement runtime change raising event to screen reader #1562
        /// <inheritdoc />                                                    
        public AccessibilityTrait[] AccessibilityTraits { get; set; }
#endif
    }
}
