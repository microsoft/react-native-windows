// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Graphics.Display;
using Windows.System;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using ReactNative.Accessibility;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using ReactNative.Modules.Core;
using ReactNative.Touch;
using ReactNative.UIManager;

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// A native control with a single ContentDialog child.
    /// </summary>
    public class ReactModalHostView : FrameworkElement, IAccessible
    {
        private ContentDialog _contentDialog;

        private bool _isLoaded;

        private bool _isClosing;

        private int _resizeCount;

        private TouchHandler _touchHandler;

        /// <inheritdoc />                                                    
        public AccessibilityTrait[] AccessibilityTraits { get; set; }

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
        public event Action<ReactModalHostView> OnRequestCloseListener;

        /// <summary>
        /// Called once the modal has been shown
        /// </summary>
        public event Action<ReactModalHostView> OnShowListener;

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
        /// Shows the dialog if not visible otherwise updates its properties
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
                    ["ContentDialogBorderWidth"] = new Thickness(0),
                    ["ContentDialogContentMargin"] = new Thickness(0),
                    ["ContentDialogContentScrollViewerMargin"] = new Thickness(0),
                    ["ContentDialogMaxHeight"] = double.NaN,
                    ["ContentDialogMaxWidth"] = double.NaN,
                    ["ContentDialogMinHeight"] = 0.0,
                    ["ContentDialogMinWidth"] = 0.0,
                    ["ContentDialogPadding"] = new Thickness(0)
                },
            };

            _contentDialog.Opened += (sender, e) =>
            {
                OnShowListener?.Invoke(this);
            };

            _contentDialog.SizeChanged += (sender, args) =>
            {
                SetContentSize();
            };

            _contentDialog.Closing += (sender, e) =>
            {
                if (_isClosing)
                {
                    _isClosing = false;
                }
                else
                {
                    e.Cancel = true;
                    OnRequestCloseListener?.Invoke(this);
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
            _isClosing = true;
            _contentDialog.Hide();
            _touchHandler.Dispose();
            Window.Current.CoreWindow.KeyDown -= CoreWindowOnKeyDown;
            SystemNavigationManager.GetForCurrentView().BackRequested -= OnBackRequested;
            DisplayInformation.GetForCurrentView().OrientationChanged -= OnOrientationChanged;
            var inputPane = InputPane.GetForCurrentView();
            inputPane.Hiding -= InputPaneOnHidingOrShowing;
            inputPane.Showing -= InputPaneOnHidingOrShowing;
            this.GetReactContext().GetNativeModule<ExceptionsManagerModule>().BeforeShowDevOptionsDialog -= Close;
        }

        /// <inheritdoc />                                              
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new DynamicAutomationPeer<ReactModalHostView>(this);
        }

        private void Show()
        {
            _contentDialog.ShowAsync().GetResults();

            Window.Current.CoreWindow.KeyDown += CoreWindowOnKeyDown;
            SystemNavigationManager.GetForCurrentView().BackRequested += OnBackRequested;
            DisplayInformation.GetForCurrentView().OrientationChanged += OnOrientationChanged;
            var inputPane = InputPane.GetForCurrentView();
            inputPane.Hiding += InputPaneOnHidingOrShowing;
            inputPane.Showing += InputPaneOnHidingOrShowing;
            this.GetReactContext().GetNativeModule<ExceptionsManagerModule>().BeforeShowDevOptionsDialog += Close;
        }

        private void CoreWindowOnKeyDown(CoreWindow sender, KeyEventArgs e)
        {
            if (e.VirtualKey == VirtualKey.Escape && !e.Handled)
            {
                _contentDialog.Hide();
            }
        }

        private async void InputPaneOnHidingOrShowing(InputPane sender, InputPaneVisibilityEventArgs args)
        {
            // Delay until the input pane has finished hiding or showing
            await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            {
                SetContentSize();
            });
        }

        private void OnBackRequested(object sender, BackRequestedEventArgs e)
        {
            e.Handled = true;
            _contentDialog.Hide();
        }

        private void OnOrientationChanged(DisplayInformation sender, object args)
        {
            SetContentSize();
        }

        /// <summary>
        /// Size the dialog content to fill the entire screen
        /// </summary>
        private void SetContentSize()
        {
            var uiManagerModule = this.GetReactContext().GetNativeModule<UIManagerModule>();
            var contentTag = Content.GetTag();
            var currentCount = ++_resizeCount;

            var contentSize = ApplicationView.GetForCurrentView().VisibleBounds;
            var inputPane = InputPane.GetForCurrentView();

            // Windows phone has resize issues if you make the modal less then half
            // the screen size and then rotate the phone
            if (inputPane.OccludedRect.Height < contentSize.Height / 2)
                contentSize.Height -= inputPane.OccludedRect.Height;

            uiManagerModule.ActionQueue.Dispatch(() =>
            {
                // If multiple events have been dispatched, ignore all but the newest
                if (currentCount == _resizeCount)
                {
                    // The modal content isn't a root node, but behaves similar to one
                    // in that it fills the entire window
                    uiManagerModule.UIImplementation.UpdateRootNodeSize(contentTag, contentSize.Width, contentSize.Height);
                }
            });
        }

        private void UpdateProperties()
        {
            if (Transparent)
            {
                _contentDialog.Background = new SolidColorBrush(Colors.Transparent);
            }
        }
    }
}
