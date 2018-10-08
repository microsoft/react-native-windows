// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if WINDOWS_UWP
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
    public class ReactModalHostView : UserControl, IAccessible
#else
    public class ReactModalHostView : UserControl
#endif
    {
        private readonly ContentDialog _contentDialog;

        private TouchHandler _touchHandler;

        /// <summary>
        /// The current dialog content
        /// </summary>
        public new DependencyObject Content
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
        /// Instantiates the <see cref="ReactModalHostView"/>. 
        /// </summary>
        public ReactModalHostView()
        {
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

            Loaded += (sender, args) =>
            {
                this._contentDialog.ShowAsync().GetResults();
            };

#if WINDOWS_UWP
            UseSystemFocusVisuals = true;
#endif
        }

        public void Hide()
        {
            _contentDialog.Hide();
            _touchHandler.Dispose();
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
