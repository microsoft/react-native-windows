// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using ReactNative.Views.Text;
using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// View manager for <see cref="PasswordBox"/>.
    /// </summary>
    class ReactPasswordBoxManager : BaseViewManager<PasswordBox, ReactPasswordBoxShadowNode>
    {
        internal static readonly Color DefaultPlaceholderTextColor = Color.FromArgb(255, 0, 0, 0);

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
            view.PasswordChar = passwordCharString.ToCharArray().First();
        }

        /// <summary>
        /// Sets the password reveal mode on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="revealModeString">The reveal mode, either Hidden, Peek, or Visibile.</param>
        [ReactProp("passwordRevealMode")]
        public void SetPasswordRevealMode(PasswordBox view, string revealModeString)
        {
            throw new NotSupportedException("Password Reveal Mode is not supported by WPF.");
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
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
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
                : new FontFamily();
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
            view.FontStyle = fontStyle ?? new FontStyle();
        }

        /// <summary>
        /// Sets the default text placeholder prop on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="placeholder">The placeholder text.</param>
        [ReactProp("placeholder")]
        public void SetPlaceholder(PasswordBox view, string placeholder)
        {
            PlaceholderAdorner.SetText(view, placeholder);
        }

        /// <summary>
        /// Sets the placeholderTextColor prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The placeholder text color.</param>
        [ReactProp("placeholderTextColor", CustomType = "Color")]
        public void SetPlaceholderTextColor(PasswordBox view, uint? color)
        {
            var brush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : new SolidColorBrush(DefaultPlaceholderTextColor);

            PlaceholderAdorner.SetTextColor(view, brush);
        }

        /// <summary>
        /// Sets the border color for the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.BorderColor, CustomType = "Color")]
        public void SetBorderColor(PasswordBox view, uint? color)
        {
            view.BorderBrush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : new SolidColorBrush(ReactTextInputManager.DefaultTextBoxBorder);
        }

        /// <summary>
        /// Sets the background color for the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.BackgroundColor, CustomType = "Color")]
        public void SetBackgroundColor(PasswordBox view, uint? color)
        {
            view.Background = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : new SolidColorBrush(Colors.White);
        }

        /// <summary>
        /// Sets the selection color for the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp("selectionColor", CustomType = "Color")]
        public void SetSelectionColor(PasswordBox view, uint color)
        {
            view.SelectionBrush = new SolidColorBrush(ColorHelpers.Parse(color));
            view.CaretBrush = new SolidColorBrush(ColorHelpers.Parse(color));
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
            var paddings = extraData as float[];
            if (paddings != null)
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
                view.Focus();
            }
            else if (commandId == ReactTextInputManager.BlurTextInput)
            {
                Keyboard.ClearFocus();
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

        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            var textBox = (PasswordBox)sender;
            if (e.Key == Key.Enter)
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
