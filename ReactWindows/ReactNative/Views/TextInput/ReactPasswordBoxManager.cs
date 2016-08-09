using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.Text;
using System;
using Windows.UI.Text;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.TextInput
{
    class ReactPasswordBoxManager : BaseViewManager<PasswordBox, ReactTextInputShadowNode>
    {
        public override string Name
        {
            get
            {
                return "PasswordBox";
            }
        }

        #region CSS

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

        #endregion

        public override ReactTextInputShadowNode CreateShadowNodeInstance()
        {
            throw new NotImplementedException();
        }

        public override void UpdateExtraData(PasswordBox root, object extraData)
        {
            throw new NotImplementedException();
        }

        protected override PasswordBox CreateViewInstance(ThemedReactContext reactContext)
        {
            throw new NotImplementedException();
        }
    }
}
