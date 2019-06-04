// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Accessibility;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using ReactNative.Views.Text;
using System;
using Windows.System;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// View manager for <see cref="ReactTextBox"/>.
    /// </summary>
    class ReactTextInputManager : BaseViewManager<ReactTextBox, ReactTextInputShadowNode>
    {
        internal const int FocusTextInput = 1;
        internal const int BlurTextInput = 2;

        //
        // Grabbed these defaults from running a UWP app.
        //

        private const uint DefaultTextControlForeground = 0xFF000000;
        private const uint DefaultTextControlForegroundPointerOver = 0xFF000000;
        private const uint DefaultTextControlForegroundFocused = 0xFF000000;
        private const uint DefaultTextControlForegroundDisabled = 0xFF7A7A7A;

        private const uint DefaultTextControlBackground = 0x66FFFFFF;
        private const uint DefaultTextControlBackgroundPointerOver = 0x99FFFFFF;
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
                return "RCTTextBox";
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
        /// The commands map for the <see cref="ReactTextInputManager"/>.
        /// </summary>
        public override JObject ViewCommandsMap
        {
            get
            {
                return new JObject
                {
                    { "focusTextInput", FocusTextInput },
                    { "blurTextInput", BlurTextInput },
                };
            }
        }

        /// <summary>
        /// Sets the font size on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="fontSize">The font size.</param>
        [ReactProp(ViewProps.FontSize)]
        public void SetFontSize(ReactTextBox view, double fontSize)
        {
            view.FontSize = fontSize;
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(ReactTextBox view, uint? color)
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
        public void SetFontFamily(ReactTextBox view, string familyName)
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
        public void SetFontWeight(ReactTextBox view, string fontWeightString)
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
        public void SetFontStyle(ReactTextBox view, string fontStyleString)
        {
            var fontStyle = EnumHelpers.ParseNullable<FontStyle>(fontStyleString);
            view.FontStyle = fontStyle ?? FontStyle.Normal;
        }

        /// <summary>
        /// Sets whether to track selection changes on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="onSelectionChange">The indicator.</param>
        [ReactProp("onSelectionChange", DefaultBoolean = false)]
        public void SetOnSelectionChange(ReactTextBox view, bool onSelectionChange)
        {
            view.OnSelectionChange = onSelectionChange;
        }

        /// <summary>
        /// Sets whether to track size changes on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="onContentSizeChange">The indicator.</param>
        [ReactProp("onContentSizeChange", DefaultBoolean = false)]
        public void setOnContentSizeChange(ReactTextBox view, bool onContentSizeChange)
        {
            view.OnContentSizeChange = onContentSizeChange;
        }

        /// <summary>
        /// Sets the selected text on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="selection">The selection.</param>
        [ReactProp("selection")]
        public void SetSelection(ReactTextBox view, JObject selection)
        {
            if (selection == null) {
                return;
            }

            var start = selection.Value<int>("start");
            var textLength = view.Text?.Length ?? 0;
            var normalizedStart = Math.Min(start, textLength);
            var end = selection.Value<int>("end");
            var selectionLength = end - start;
            var normalizedSelectionLength = Math.Max(selectionLength, 0);
            var maxLength = textLength - normalizedStart;
            view.SelectionStart = normalizedStart;
            view.SelectionLength = Math.Min(normalizedSelectionLength, maxLength);
        }

        /// <summary>
        /// Sets the default text placeholder prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="placeholder">The placeholder text.</param>
        [ReactProp("placeholder")]
        public void SetPlaceholder(ReactTextBox view, string placeholder)
        {
            view.PlaceholderText = placeholder;
        }

        /// <summary>
        /// Sets the placeholderTextColor prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The placeholder text color.</param>
        [ReactProp("placeholderTextColor", CustomType = "Color")]
        public void SetPlaceholderTextColor(ReactTextBox view, uint? color)
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
        public void SetBorderColor(ReactTextBox view, uint? color)
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
        public void SetBackgroundColor(ReactTextBox view, uint? color)
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
        /// Sets the selection color for the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp("selectionColor", CustomType = "Color")]
        public void SetSelectionColor(ReactTextBox view, uint color)
        {
            view.SelectionHighlightColor = new SolidColorBrush(ColorHelpers.Parse(color));
        }

        /// <summary>
        /// Sets the text alignment prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="alignment">The text alignment.</param>
        [ReactProp(ViewProps.TextAlign)]
        public void SetTextAlign(ReactTextBox view, string alignment)
        {
            view.TextAlignment = EnumHelpers.Parse<TextAlignment>(alignment);
        }

        /// <summary>
        /// Sets the text alignment prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="alignment">The text alignment.</param>
        [ReactProp(ViewProps.TextAlignVertical)]
        public void SetTextVerticalAlign(ReactTextBox view, string alignment)
        {
            view.VerticalContentAlignment = EnumHelpers.Parse<VerticalAlignment>(alignment);
        }

        /// <summary>
        /// Sets the editablity prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="editable">The editable flag.</param>
        [ReactProp("editable")]
        public void SetEditable(ReactTextBox view, bool editable)
        {
            view.IsEnabled = editable;
        }

        /// <summary>
        /// Sets the max character length prop on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="maxCharLength">The max length.</param>
        [ReactProp("maxLength")]
        public void SetMaxLength(ReactTextBox view, int maxCharLength)
        {
            view.MaxLength = maxCharLength;
        }

        /// <summary>
        /// Sets whether to enable autocorrect on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="autoCorrect">The autocorrect flag.</param>
        [ReactProp("autoCorrect")]
        public void SetAutoCorrect(ReactTextBox view, bool autoCorrect)
        {
            view.IsSpellCheckEnabled = autoCorrect;
        }

        /// <summary>
        /// Sets whether to enable multiline input on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="multiline">The multiline flag.</param>
        [ReactProp("multiline", DefaultBoolean = false)]
        public void SetMultiline(ReactTextBox view, bool multiline)
        {
            view.AcceptsReturn = multiline;
            view.TextWrapping = multiline ? TextWrapping.Wrap : TextWrapping.NoWrap;
        }

        /// <summary>
        /// Sets whether to enable the <see cref="ReactTextBox"/> to autogrow.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="autoGrow">The auto-grow flag.</param>
        [ReactProp("autoGrow", DefaultBoolean = false)]
        public void SetAutoGrow(ReactTextBox view, bool autoGrow)
        {
            view.AutoGrow = autoGrow;
            if (autoGrow)
            {
                view.Height = double.NaN;
            }
        }

        /// <summary>
        /// Sets the keyboard type on the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="keyboardType">The keyboard type.</param>
        [ReactProp("keyboardType")]
        public void SetKeyboardType(ReactTextBox view, string keyboardType)
        {
            view.InputScope = null;
            if (keyboardType != null)
            {
                var inputScope = new InputScope();
                inputScope.Names.Add(
                    new InputScopeName(
                        InputScopeHelpers.FromString(keyboardType)));

                view.InputScope = inputScope;
            }
        }

        /// <summary>
        /// Sets the border width for a <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="width">The border width.</param>
        [ReactProp(ViewProps.BorderWidth)]
        public void SetBorderWidth(ReactTextBox view, int width)
        {
            view.BorderThickness = new Thickness(width);
        }

        /// <summary>
        /// Sets whether the text should be cleared on focus.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="clearTextOnFocus">The indicator.</param>
        [ReactProp("clearTextOnFocus")]
        public void SetClearTextOnFocus(ReactTextBox view, bool clearTextOnFocus)
        {
            view.ClearTextOnFocus = clearTextOnFocus;
        }

        /// <summary>
        /// Sets whether the text should be selected on focus.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="selectTextOnFocus">The indicator.</param>
        [ReactProp("selectTextOnFocus")]
        public void SetSelectTextOnFocus(ReactTextBox view, bool selectTextOnFocus)
        {
            view.SelectTextOnFocus = selectTextOnFocus;
        }

        /// <summary>
        /// Sets the max height of the text box.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="height">The max height.</param>
        [ReactProp("maxHeight")]
        public void SetMaxHeight(ReactTextBox view, double height)
        {
            view.MaxHeight = height;
        }

        /// <summary>
        /// Sets whether the view is a tab stop.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="isTabStop">
        /// <code>true</code> if the view is a tab stop, otherwise <code>false</code> (control can't get keyboard focus or accept keyboard input in this case).
        /// </param>
        [ReactProp("isTabStop")]
        public void SetIsTabStop(ReactTextBox view, bool isTabStop)
        {
            view.IsTabStop = isTabStop;
        }

        /// <summary>
        /// Sets the tab index for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="tabIndex">The tab index.</param>
        [ReactProp("tabIndex")]
        public void SetTabIndex(ReactTextBox view, int tabIndex)
        {
            view.TabIndex = tabIndex;
        }

        /// <summary>
        /// Sets <see cref="ImportantForAccessibility"/> for ReactTextBox.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="importantForAccessibilityValue">The string to be parsed as <see cref="ImportantForAccessibility"/>.</param>
        [ReactProp(ViewProps.ImportantForAccessibility)]
        public void SetImportantForAccessibility(ReactTextBox view, string importantForAccessibilityValue)
        {
            var importantForAccessibility = EnumHelpers.ParseNullable<ImportantForAccessibility>(importantForAccessibilityValue) ?? ImportantForAccessibility.Auto;
            AccessibilityHelper.SetImportantForAccessibility(view, importantForAccessibility);
        }

        /// <summary>
        /// Controls the visibility of the DeleteButton.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="clearButtonMode">Visibility of the DeleteButton.</param>
        [ReactProp("clearButtonMode")]
        public void SetClearButtonMode(ReactTextBox view, string clearButtonMode)
        {
            view.ClearButtonMode = EnumHelpers.ParseNullable<ClearButtonModeType>(clearButtonMode) ?? ClearButtonModeType.Default;
        }

        /// <summary>
        /// Create the shadow node instance.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactTextInputShadowNode CreateShadowNodeInstance()
        {
            return new ReactTextInputShadowNode();
        }

        /// <summary>
        /// Implement this method to receive events/commands directly from
        /// JavaScript through the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(ReactTextBox view, int commandId, JArray args)
        {
            if (commandId == FocusTextInput)
            {
                view.Focus(FocusState.Programmatic);
            }
            else if (commandId == BlurTextInput)
            {
                if (FocusManager.GetFocusedElement() == view)
                {
                    var frame = Window.Current?.Content as Frame;
                    frame?.Focus(FocusState.Programmatic);
                }
            }
        }

        /// <summary>
        /// Update the view with extra data.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(ReactTextBox view, object extraData)
        {
            if (extraData is float[] paddings)
            {
                view.Padding = new Thickness(
                    paddings[0],
                    paddings[1],
                    paddings[2],
                    paddings[3]);
            }
            else if (extraData is Tuple<int, string> textUpdate)
            {
                var javaScriptCount = textUpdate.Item1;
                if (javaScriptCount < view.CurrentEventCount)
                {
                    return;
                }

                view.TextChanging -= OnTextChanging;
                view.TextChanged -= OnTextChanged;

                var removeOnSelectionChange = view.OnSelectionChange;
                if (removeOnSelectionChange)
                {
                    view.OnSelectionChange = false;
                }

                var text = textUpdate.Item2;
                var previousText = view.Text;

                var selectionStart = view.SelectionStart;
                var textLength = text?.Length ?? 0;
                var normalizedStart = Math.Min(selectionStart, textLength);
                var selectionLength = view.SelectionLength;
                var maxLength = textLength - normalizedStart;

                view.Text = text ?? "";
                if (selectionStart == previousText.Length)
                {
                    view.SelectionStart = textLength;
                }
                else
                {
                    view.SelectionStart = normalizedStart;
                    view.SelectionLength = Math.Min(selectionLength, maxLength);
                }

                if (removeOnSelectionChange)
                {
                    view.OnSelectionChange = true;
                }

                view.TextChanged += OnTextChanged;
                view.TextChanging += OnTextChanging;
            }
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup by the <see cref="ReactTextInputManager"/>.
        /// subclass. Unregister all event handlers for the <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The <see cref="ReactTextBox"/>.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, ReactTextBox view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.KeyDown -= OnKeyDown;
            view.LostFocus -= OnLostFocus;
            view.GotFocus -= OnGotFocus;
            view.TextChanged -= OnTextChanged;
            view.TextChanging -= OnTextChanging;
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The dimensions.</param>
        public override void SetDimensions(ReactTextBox view, Dimensions dimensions)
        {
            view.MinWidth = dimensions.Width;
            view.MinHeight = dimensions.Height;

            if (view.AutoGrow)
            {
                // TODO: investigate Yoga bug that rounds up height 1px
                view.DimensionsUpdated = true;
                Canvas.SetLeft(view, dimensions.X);
                Canvas.SetTop(view, dimensions.Y);
                view.Width = dimensions.Width;
            }
            else
            {
                base.SetDimensions(view, dimensions);
            }
        }

        /// <summary>
        /// Returns the view instance for <see cref="ReactTextBox"/>.
        /// </summary>
        /// <param name="reactContext"></param>
        /// <returns></returns>
        protected override ReactTextBox CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ReactTextBox
            {
                AcceptsReturn = false,
            };
        }

        /// <summary>
        /// Installing the textchanged event emitter on the <see cref="TextInput"/> Control.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The <see cref="ReactTextBox"/> view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, ReactTextBox view)
        {
            base.AddEventEmitters(reactContext, view);
            view.TextChanging += OnTextChanging;
            view.TextChanged += OnTextChanged;
            view.GotFocus += OnGotFocus;
            view.LostFocus += OnLostFocus;
            view.KeyDown += OnKeyDown;
        }

        private void OnTextChanging(TextBox sender, TextBoxTextChangingEventArgs args)
        {
            var textBox = (ReactTextBox)sender;
            textBox.IncrementEventCount();
        }

        private void OnTextChanged(object sender, TextChangedEventArgs e)
        {
            var textBox = (ReactTextBox)sender;
            textBox.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextChangedEvent(
                        textBox.GetTag(),
                        textBox.Text,
                        textBox.CurrentEventCount));
        }

        private void OnGotFocus(object sender, RoutedEventArgs e)
        {
            var textBox = (ReactTextBox)sender;
            textBox.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new FocusEvent(textBox.GetTag()));
        }

        private void OnLostFocus(object sender, RoutedEventArgs e)
        {
            var textBox = (ReactTextBox)sender;
            var eventDispatcher = textBox.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher;

            eventDispatcher.DispatchEvent(
                new BlurEvent(textBox.GetTag()));

            eventDispatcher.DispatchEvent(
                new ReactTextInputEndEditingEvent(
                      textBox.GetTag(),
                      textBox.Text));
        }

        private void OnKeyDown(object sender, KeyRoutedEventArgs e)
        {
            var textBox = (ReactTextBox)sender;
            if (e.Key == VirtualKey.Enter)
            {
                if (!textBox.AcceptsReturn)
                {
                    e.Handled = true;
                    textBox.GetReactContext()
                        .GetNativeModule<UIManagerModule>()
                        .EventDispatcher
                        .DispatchEvent(
                            new ReactTextInputSubmitEditingEvent(
                                textBox.GetTag(),
                                textBox.Text));
                }
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
