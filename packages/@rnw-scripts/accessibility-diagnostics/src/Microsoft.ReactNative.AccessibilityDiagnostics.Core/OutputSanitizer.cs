// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Text;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core;

public static class OutputSanitizer
{
    public static string Sanitize(string value)
    {
        ArgumentNullException.ThrowIfNull(value);

        var builder = new StringBuilder(value.Length);
        foreach (var character in value)
        {
            if (char.IsControl(character))
            {
                builder.Append($"\\u{(int)character:X4}");
            }
            else
            {
                builder.Append(character);
            }
        }

        return builder.ToString();
    }
}
