// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
            if (int.TryParse(fontWeightString, out var result))
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
    }
}
