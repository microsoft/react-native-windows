// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
#if WINDOWS_UWP
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
#endif

namespace ReactNative.Views.Text
{
#if WINDOWS_UWP
    using Key = VirtualKey;
#endif

    /// <summary>
    /// A view manager class for hyperlink text nodes.
    /// </summary>
    public class ReactHyperlinkViewManager : ReactSpanViewManager
    {
        private const int FocusCommand = 1;
        private const int BlurCommand = 2;

        private const string ClickEventString = "hyperlinkClick";

        /// <summary>
        /// The view manager name.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTHyperlink";
            }
        }

        /// <summary>
        /// Valid commands for the view.
        /// </summary>
        public override JObject ViewCommandsMap
        {
            get
            {
                return new JObject
                {
                    { "focus", FocusCommand },
                    { "blur", BlurCommand },
                };
            }
        }

        /// <summary>
        /// Event mapping for events sent by the view.
        /// </summary>
        public override JObject CustomDirectEventTypeConstants => new JObject
        {
            {
                ClickEventString, new JObject
                {
                    { "registrationName", "onClick" }
                }
            }
        };

        /// <summary>
        /// Creates a new view instance of type <see cref="HyperlinkButton"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Span CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Hyperlink();
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup by the <see cref="ReactHyperlinkViewManager"/>.
        /// subclass. Unregister all event handlers for the <see cref="Hyperlink"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The <see cref="Hyperlink"/> view instance as a <see cref="Span"/>.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, Span view)
        {
            var hyperlink = (Hyperlink)view;

            base.OnDropViewInstance(reactContext, view);
            hyperlink.GotFocus -= OnGotFocus;
            hyperlink.LostFocus -= OnLostFocus;
            hyperlink.Click -= OnClick;
        }

        /// <summary>
        /// Installing the click and focus event emitters on the <see cref="Hyperlink"/> Control.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The <see cref="Hyperlink"/> view instance as a <see cref="Span"/>.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, Span view)
        {
            var hyperlink = (Hyperlink)view;

            base.AddEventEmitters(reactContext, view);
            hyperlink.GotFocus += OnGotFocus;
            hyperlink.LostFocus -= OnLostFocus;
            hyperlink.Click += OnClick;
        }

        private void OnGotFocus(object sender, RoutedEventArgs e)
        {
            if (e.OriginalSource == sender)
            {
                var view = (Hyperlink)sender;
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(new FocusEvent(view.GetTag()));
            }
        }

        private void OnLostFocus(object sender, RoutedEventArgs e)
        {
            if (e.OriginalSource == sender)
            {
                var view = (Hyperlink)sender;
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(new BlurEvent(view.GetTag()));
            }
        }

        private void OnClick(object view, RoutedEventArgs args)
        {
            // send a key enter event here, which will be interpreted as a click
            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new KeyEvent(ClickEventString, view.GetTag(), Key.Enter));
        }

        /// <summary>
        /// Receive a command from JavaScript.
        /// </summary>
        /// <param name="view">The <see cref="Hyperlink"/> view instance as a <see cref="Span"/>.</param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(Span view, int commandId, JArray args)
        {
            var hyperlink = (Hyperlink)view;

            // run on dispatcher to avoid reentrancy in OnGotFocus / OnLostFocus.
            DispatcherHelpers.RunOnDispatcher(async () =>
            {
                if (commandId == FocusCommand)
                {
#if WINDOWS_UWP
                    hyperlink.Focus(FocusState.Programmatic);
#else
                    hyperlink.Focus();
#endif
                }
                else if (commandId == BlurCommand)
                {
#if WINDOWS_UWP
                    var frame = Window.Current?.Content as Frame;
                    frame?.Focus(FocusState.Programmatic);
#else
                    Keyboard.ClearFocus();
#endif
                }
            });
        }
    }
}
