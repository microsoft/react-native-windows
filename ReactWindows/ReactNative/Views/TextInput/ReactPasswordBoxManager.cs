using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.Text;
using System;
using System.Collections.Generic;
using Windows.UI;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.TextInput
{
    class ReactPasswordBoxManager : BaseViewManager<PasswordBox, ReactPasswordBoxShadowNode>
    {
        private const int FocusTextInput = 1;
        private const int BlurTextInput = 2;

        public override string Name
        {
            get
            {
                return "PasswordBox";
            }
        }

        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        public override IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
        {
            get
            {
                return new Dictionary<string, object>()
                {
                    {
                        "topSubmitEditing",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onSubmitEditing" },
                                    { "captured" , "onSubmitEditingCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topEndEditing",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onEndEditing" },
                                    { "captured" , "onEndEditingCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topFocus",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onFocus" },
                                    { "captured" , "onFocusCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topBlur",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onBlur" },
                                    { "captured" , "onBlurCapture" }
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
        public override IReadOnlyDictionary<string, object> CommandsMap
        {
            get
            {
                return new Dictionary<string, object>()
                {
                    { "focusTextInput", FocusTextInput },
                    { "blurTextInput", BlurTextInput },
                };
            }
        }

        #region CSS

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
        /// Sets the default text placeholder property on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="placeholder">The placeholder text.</param>
        [ReactProp("placeholder")]
        public void SetPlaceholder(PasswordBox view, string placeholder)
        {
            view.PlaceholderText = placeholder;
        }

        /// <summary>
        /// Sets the background color for the <see cref="PasswordBox"/>.
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
            view.SelectionHighlightColor = new SolidColorBrush(ColorHelpers.Parse(color));
        }

        /// <summary>
        /// Sets the text alignment property on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="alignment">The text alignment.</param>
        [ReactProp(ViewProps.TextAlignVertical)]
        public void SetTextVerticalAlign(PasswordBox view, string alignment)
        {
            view.VerticalContentAlignment = EnumHelpers.Parse<VerticalAlignment>(alignment);
        }

        /// <summary>
        /// Sets the editablity property on the <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="editable">The editable flag.</param>
        [ReactProp("editable")]
        public void SetEditable(PasswordBox view, bool editable)
        {
            view.IsEnabled = editable;
        }

        /// <summary>
        /// Sets the max character length property on the <see cref="PasswordBox"/>.
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
        /// Sets the border width for a <see cref="PasswordBox"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="width">The border width.</param>
        [ReactProp(ViewProps.BorderWidth)]
        public void SetBorderWidth(PasswordBox view, int width)
        {
            view.BorderThickness = new Thickness(width);
        }

        #endregion

        public override ReactPasswordBoxShadowNode CreateShadowNodeInstance()
        {
            return new ReactPasswordBoxShadowNode();
        }

        public override void UpdateExtraData(PasswordBox root, object extraData)
        {
            
        }

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
            if (commandId == FocusTextInput)
            {
                view.Focus(FocusState.Programmatic);
            }
            else if (commandId == BlurTextInput)
            {
                var frame = Window.Current?.Content as Frame;
                frame?.Focus(FocusState.Programmatic);
            }
        }
    }
}
