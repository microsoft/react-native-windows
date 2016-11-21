#if WINDOWS_UWP
using Windows.UI.Text;
#else
using System.Windows;
#endif

namespace ReactNative.Views.Text
{
    static class FontStyleHelpers
    {
        public static FontWeight? ParseFontWeight(string fontWeightString)
        {
            var fontWeightNumeric = fontWeightString != null
                ? ParseNumericFontWeight(fontWeightString)
                : -1;

            if (fontWeightNumeric > ushort.MaxValue)
            {
                return FontWeights.ExtraBold;
            }
            else if (fontWeightNumeric > 0)
            {
                return ConvertNumeric(fontWeightNumeric);
            }
            else if (fontWeightString == "bold")
            {
                return FontWeights.Bold;
            }
            else if (fontWeightString == "normal")
            {
                return FontWeights.Normal;
            }

            return null;
        }

        private static int ParseNumericFontWeight(string fontWeightString)
        {
            var result = default(int);
            if (int.TryParse(fontWeightString, out result))
            {
                return result;
            }

            return -1;
        }

        private static FontWeight ConvertNumeric(int fontWeightNumeric)
        {
#if WINDOWS_UWP
            return new FontWeight
                {
                    Weight = (ushort)fontWeightNumeric,
                };
#else
            if (fontWeightNumeric >= 0 && fontWeightNumeric <= 149)
            {
                return FontWeights.Thin;
            }
            else if (fontWeightNumeric >= 150 && fontWeightNumeric <= 249)
            {
                return FontWeights.ExtraLight;
            }
            else if (fontWeightNumeric >= 250 && fontWeightNumeric <= 349)
            {
                return FontWeights.Light;
            }
            else if (fontWeightNumeric >= 350 && fontWeightNumeric <= 449)
            {
                return FontWeights.Normal;
            }
            else if (fontWeightNumeric >= 450 && fontWeightNumeric <= 549)
            {
                return FontWeights.Medium;
            }
            else if (fontWeightNumeric >= 550 && fontWeightNumeric <= 649)
            {
                return FontWeights.SemiBold;
            }
            else if (fontWeightNumeric >= 650 && fontWeightNumeric <= 749)
            {
                return FontWeights.Bold;
            }
            else if (fontWeightNumeric >= 750 && fontWeightNumeric <= 849)
            {
                return FontWeights.ExtraBold;
            }
            else if (fontWeightNumeric >= 850 && fontWeightNumeric <= 949)
            {
                return FontWeights.Black;
            }
            else if (fontWeightNumeric >= 950 && fontWeightNumeric <= 999)
            {
                return FontWeights.ExtraBlack;
            }
            else
            {
                return FontWeights.Normal;
            }
#endif
        }

#if !WINDOWS_UWP
        public static FontStyle? ConvertFontStyle(string fontStyleValue)
        {
            FontStyle? fontStyle = null;

            if (!string.IsNullOrEmpty(fontStyleValue))
            {
                var converter = new FontStyleConverter();

                var convertFrom = converter.ConvertFromInvariantString(fontStyleValue);
                if (convertFrom != null)
                {
                    var convertedFontStyle = (FontStyle)convertFrom;

                    fontStyle = convertedFontStyle;
                }
            }

            return fontStyle;
        }

        public static FontStretch? ConvertFontStretch(int usWidthClass)
        {
            FontStretch? fontStretch = null;

            switch (usWidthClass)
            {
                case 1:
                    fontStretch = FontStretches.UltraCondensed;
                    break;
                case 2:
                    fontStretch = FontStretches.ExtraCondensed;
                    break;
                case 3:
                    fontStretch = FontStretches.Condensed;
                    break;
                case 4:
                    fontStretch = FontStretches.SemiCondensed;
                    break;
                case 5:
                    fontStretch = FontStretches.Medium;
                    break;
                case 6:
                    fontStretch = FontStretches.SemiExpanded;
                    break;
                case 7:
                    fontStretch = FontStretches.Expanded;
                    break;
                case 8:
                    fontStretch = FontStretches.ExtraExpanded;
                    break;
                case 9:
                    fontStretch = FontStretches.UltraExpanded;
                    break;
            }
            
            return fontStretch;
        }

        public static FontStretch? ConvertFontStretch(string fontStretchValue)
        {
            FontStretch? fontStretch = null;

            if (!string.IsNullOrEmpty(fontStretchValue))
            {
                var converter = new FontStretchConverter();

                var convertFrom = converter.ConvertFromInvariantString(fontStretchValue);
                if (convertFrom != null)
                {
                    var convertedFontStretch = (FontStretch)convertFrom;

                    fontStretch = convertedFontStretch;
                }
            }

            return fontStretch;
        }

#endif

    }
}
