// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using ReactNative.Views.Text;
using Windows.System;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// View manager for <see cref="PasswordBox"/>.
    /// </summary>
    class ReactPasswordBoxManager : BaseViewManager<PasswordBox, ReactPasswordBoxShadowNode>
    {
        private const uint DefaultTextControlForeground = 0xFF000000;
        private const uint DefaultTextControlForegroundPointerOver = 0xFF000000;
        private const uint DefaultTextControlForegroundFocused = 0xFF000000;
        private const uint DefaultTextControlForegroundDisabled = 0xFF7A7A7A;

        private const uint DefaultTextControlBackground = 0x66FFFFFF;
        private const uint DefaultTextControlBackgroundPointerOver = 0xFFFFFFFF;
        private const uint DefaultTextControlBackgroundFocused = 0xFFFFFFFF;
        private const uint DefaultTextControlBackgroundDisabled = 0x33000000;

        private const uint DefaultTextControlPlaceholderForeground = 0x99000000;
        private const uint DefaultTextControlPlaceholderForegroundPointerOver = 0x99000000;
        private const uint DefaultTextControlPlaceholderForegroundFocused = 0x66000000;
        private const uint DefaultTextControlPlaceholderForegroundDisabled = 0xFF7A7A7A;

        private const uint DefaultTextControlBorderBrush = 0xFF7A7A7A;
        private const uint DefaultTextControlBorderBrushPointerOver = 0xFF171717;
        private const uint DefaultTextControlBorderBrushFocused = 0xFF298FCC;
        private const uint DefaultTextControlBorderBrushDisabled = 0x33000000;

        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "PasswordBoxWindows";
            }
        }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        public override JObject CustomBubblingEventTypeConstants
        {
            get
            {
                return new JObject
                {
                    {
                        "topSubmitEditing",
                        new JObject
                        {
                            {
                                "phasedRegistrationNames",
                                new JObject
                                {
                                    { "bubbled" , "onSubmitEditing" },
                                    { "captured" , "onSubmitEditingCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topEndEditing",
                        new JObject
                        {
                            {
                                "phasedRegistrationNames",
                                new JObject
                                {
                                    { "bubbled" , "onEndEditing" },
                                    { "captured" , "onEndEditingCapture" }
                                }
                            }
                        }
                    },
                };
            }
        }

        /// <summary>
        /// Sets the password character on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="passwordCharString">The password masking character to set.</param>
        [ReactProp("passwordChar")]
        public void SetPasswordChar(PasswordBox view, string passwordCharString)
        {
            view.PasswordChar = passwordCharString;
        }

        /// <summary>
        /// Sets the password reveal mode on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="revealModeString">The reveal mode, either Hidden, Peek, or Visibile.</param>
        [ReactProp("passwordRevealMode")]
        public void SetPasswordRevealMode(PasswordBox view, string revealModeString)
        {
            var revealMode = EnumHelpers.ParseNullable<PasswordRevealMode>(revealModeString);
            view.PasswordRevealMode = revealMode ?? PasswordRevealMode.Peek;
        }

        /// <summary>
        /// Sets the font size on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="fontSize">The font size.</param>
        [ReactProp(ViewProps.FontSize)]
        public void SetFontSize(PasswordBox view, double fontSize)
        {
            view.FontSize = fontSize;
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(PasswordBox view, uint? color)
        {
            if (color.HasValue)
            {
                var brush = new SolidColorBrush(ColorHelpers.Parse(color.Value));
                view.Resources["TextControlForeground"] = brush;
                view.Resources["TextControlForegroundPointerOver"] = brush;
                view.Resources["TextControlForegroundFocused"] = brush;
                view.Resources["TextControlForegroundDisabled"] = brush;
            }
            else
            {
                view.Resources["TextControlForeground"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlForeground));
                view.Resources["TextControlForegroundPointerOver"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlForegroundPointerOver));
                view.Resources["TextControlForegroundFocused"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlForegroundFocused));
                view.Resources["TextControlForegroundDisabled"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlForegroundDisabled));
            }
        }

        /// <summary>
        /// Sets the font family for the node.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="familyName">The font family.</param>
        [ReactProp(ViewProps.FontFamily)]
        public void SetFontFamily(PasswordBox view, string familyName)
        {
            view.FontFamily = familyName != null
                ? new FontFamily(familyName)
                : FontFamily.XamlAutoFontFamily;
        }

        /// <summary>
        /// Sets the font weight for the node.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="fontWeightString">The font weight string.</param>
        [ReactProp(ViewProps.FontWeight)]
        public void SetFontWeight(PasswordBox view, string fontWeightString)
        {
            var fontWeight = FontStyleHelpers.ParseFontWeight(fontWeightString);
            view.FontWeight = fontWeight ?? FontWeights.Normal;
        }

        /// <summary>
        /// Sets the font style for the node.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="fontStyleString">The font style string.</param>
        [ReactProp(ViewProps.FontStyle)]
        public void SetFontStyle(PasswordBox view, string fontStyleString)
        {
            var fontStyle = EnumHelpers.ParseNullable<FontStyle>(fontStyleString);
            view.FontStyle = fontStyle ?? FontStyle.Normal;
        }

        /// <summary>
        /// Sets the default text placeholder prop on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="placeholder">The placeholder text.</param>
        [ReactProp("placeholder")]
        public void SetPlaceholder(PasswordBox view, string placeholder)
        {
            view.PlaceholderText = placeholder;
        }

        /// <summary>
        /// Sets the placeholderTextColor prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The placeholder text color.</param>
        [ReactProp("placeholderTextColor", CustomType = "Color")]
        public void SetPlaceholderTextColor(PasswordBox view, uint? color)
        {
            if (color.HasValue)
            {
                var brush = new SolidColorBrush(ColorHelpers.Parse(color.Value));
                view.Resources["TextControlPlaceholderForeground"] = brush;
                view.Resources["TextControlPlaceholderForegroundPointerOver"] = brush;
                view.Resources["TextControlPlaceholderForegroundFocused"] = brush;
                view.Resources["TextControlPlaceholderForegroundDisabled"] = brush;
            }
            else
            {
                view.Resources["TextControlPlaceholderForeground"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlPlaceholderForeground));
                view.Resources["TextControlPlaceholderForegroundPointerOver"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlPlaceholderForegroundPointerOver));
                view.Resources["TextControlPlaceholderForegroundFocused"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlPlaceholderForegroundFocused));
                view.Resources["TextControlPlaceholderForegroundDisabled"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlPlaceholderForegroundDisabled));
            }
        }

        /// <summary>
        /// Sets the border color for the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.BorderColor, CustomType = "Color")]
        public void SetBorderColor(PasswordBox view, uint? color)
        {
            if (color.HasValue)
            {
                var brush = new SolidColorBrush(ColorHelpers.Parse(color.Value));
                view.Resources["TextControlBorderBrush"] = brush;
                view.Resources["TextControlBorderBrushPointerOver"] = brush;
                view.Resources["TextControlBorderBrushFocused"] = brush;
                view.Resources["TextControlBorderBrushDisabled"] = brush;
            }
            else
            {
                view.Resources["TextControlBorderBrush"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBorderBrush));
                view.Resources["TextControlBorderBrushPointerOver"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBorderBrushPointerOver));
                view.Resources["TextControlBorderBrushFocused"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBorderBrushFocused));
                view.Resources["TextControlBorderBrushDisabled"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBorderBrushDisabled));
            }
        }

        /// <summary>
        /// Sets the background color for the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.BackgroundColor, CustomType = "Color")]
        public void SetBackgroundColor(PasswordBox view, uint? color)
        {
            if (color.HasValue)
            {
                var brush = new SolidColorBrush(ColorHelpers.Parse(color.Value));
                view.Resources["TextControlBackground"] = brush;
                view.Resources["TextControlBackgroundPointerOver"] = brush;
                view.Resources["TextControlBackgroundFocused"] = brush;
                view.Resources["TextControlBackgroundDisabled"] = brush;
            }
            else
            {
                view.Resources["TextControlBackground"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBackground));
                view.Resources["TextControlBackgroundPointerOver"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBackgroundPointerOver));
                view.Resources["TextControlBackgroundFocused"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBackgroundFocused));
                view.Resources["TextControlBackgroundDisabled"] = new SolidColorBrush(ColorHelpers.Parse(DefaultTextControlBackgroundDisabled));
            }
        }

        /// <summary>
        /// Sets the selection color for the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp("selectionColor", CustomType = "Color")]
        public void SetSelectionColor(PasswordBox view, uint color)
        {
            view.SelectionHighlightColor = new SolidColorBrush(ColorHelpers.Parse(color));
        }

        /// <summary>
        /// Sets the text alignment prop on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="alignment">The text alignment.</param>
        [ReactProp(ViewProps.TextAlignVertical)]
        public void SetTextVerticalAlign(PasswordBox view, string alignment)
        {
            view.VerticalContentAlignment = EnumHelpers.Parse<VerticalAlignment>(alignment);
        }

        /// <summary>
        /// Sets the editablity prop on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="editable">The editable flag.</param>
        [ReactProp("editable")]
        public void SetEditable(PasswordBox view, bool editable)
        {
            view.IsEnabled = editable;
        }

        /// <summary>
        /// Sets whether the view is a tab stop.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="isTabStop">
        /// <code>true</code> if the view is a tab stop, otherwise <code>false</code> (control can't get keyboard focus or accept keyboard input in this case).
        /// </param>
        [ReactProp("isTabStop")]
        public void SetIsTabStop(PasswordBox view, bool isTabStop)
        {
            view.IsTabStop = isTabStop;
        }

        /// <summary>
        /// Sets the tab index for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="tabIndex">The tab index.</param>
        [ReactProp("tabIndex")]
        public void SetTabIndex(PasswordBox view, int tabIndex)
        {
            view.TabIndex = tabIndex;
        }

        /// <summary>
        /// Sets the max character length prop on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="maxCharLength">The max length.</param>
        [ReactProp("maxLength")]
        public void SetMaxLength(PasswordBox view, int maxCharLength)
        {
            view.MaxLength = maxCharLength;
        }

        /// <summary>
        /// Sets the keyboard type on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="keyboardType">The keyboard type.</param>
        [ReactProp("keyboardType")]
        public void SetKeyboardType(PasswordBox view, string keyboardType)
        {
            var inputScope = new InputScope();
            var nameValue = keyboardType != null
                ? InputScopeHelpers.FromStringForPasswordBox(keyboardType)
                : InputScopeNameValue.Password;

            inputScope.Names.Add(new InputScopeName(nameValue));
            view.InputScope = inputScope;
        }

        /// <summary>
        /// Sets the border width for a <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="width">The border width.</param>
        [ReactProp(ViewProps.BorderWidth)]
        public void SetBorderWidth(PasswordBox view, int width)
        {
            view.BorderThickness = new Thickness(width);
        }

        public override ReactPasswordBoxShadowNode CreateShadowNodeInstance()
        {
            return new ReactPasswordBoxShadowNode();
        }

        /// <summary>
        /// Update the view with extra data.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(PasswordBox view, object extraData)
        {
            if (extraData is float[] paddings)
            {
                view.Padding = new Thickness(
                    paddings[0],
                    paddings[1],
                    paddings[2],
                    paddings[3]);
            }
        }

        /// <summary>
        /// Returns the view instance for <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="reactContext">The themed React Context</param>
        /// <returns>A new initialized <see cref="PasswordBox"/></returns>
        protected override PasswordBox CreateViewInstance(ThemedReactContext reactContext)
        {
            return new PasswordBox();
        }

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(PasswordBox view, int commandId, JArray args)
        {
            if (commandId == ReactTextInputManager.FocusTextInput)
            {
                view.Focus(FocusState.Programmatic);
            }
            else if (commandId == ReactTextInputManager.BlurTextInput)
            {
                if (FocusManager.GetFocusedElement() == view)
                {
                    var frame = Window.Current?.Content as Frame;
                    frame?.Focus(FocusState.Programmatic);
                }
            }
        }

        /// <summary>
        /// Installing the textchanged event emitter on the <see cref="TextInput"/> Control.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The <see cref="PasswordBox"/> view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, PasswordBox view)
        {
            base.AddEventEmitters(reactContext, view);
            view.PasswordChanged += OnPasswordChanged;
            view.GotFocus += OnGotFocus;
            view.LostFocus += OnLostFocus;
            view.KeyDown += OnKeyDown;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup by the <see cref="ReactTextInputManager"/>.
        /// subclass. Unregister all event handlers for the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The <see cref="PasswordBox"/>.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, PasswordBox view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.KeyDown -= OnKeyDown;
            view.LostFocus -= OnLostFocus;
            view.GotFocus -= OnGotFocus;
            view.PasswordChanged -= OnPasswordChanged;
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        public override void SetDimensions(PasswordBox view, Dimensions dimensions)
        {
            base.SetDimensions(view, dimensions);
            view.MinWidth = dimensions.Width;
            view.MinHeight = dimensions.Height;
        }

        private void OnPasswordChanged(object sender, RoutedEventArgs e)
        {
            var textBox = (PasswordBox)sender;
            textBox.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextChangedEvent(
                        textBox.GetTag(),
                        textBox.Password,
                        0));
        }

        private void OnGotFocus(object sender, RoutedEventArgs e)
        {
            var textBox = (PasswordBox)sender;
            textBox.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new FocusEvent(textBox.GetTag()));
        }

        private void OnLostFocus(object sender, RoutedEventArgs e)
        {
            var textBox = (PasswordBox)sender;
            var eventDispatcher = textBox.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher;

            eventDispatcher.DispatchEvent(
                new BlurEvent(textBox.GetTag()));

            eventDispatcher.DispatchEvent(
                new ReactTextInputEndEditingEvent(
                      textBox.GetTag(),
                      textBox.Password));
        }

        private void OnKeyDown(object sender, KeyRoutedEventArgs e)
        {
            var textBox = (PasswordBox)sender;
            if (e.Key == VirtualKey.Enter)
            {
                e.Handled = true;
                textBox.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new ReactTextInputSubmitEditingEvent(
                            textBox.GetTag(),
                            textBox.Password));
            }

            if (!e.Handled)
            {
                textBox.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new KeyEvent(
                            KeyEvent.KeyPressEventString,
                            textBox.GetTag(),
                            e.Key));
            }
        }
    }
}
