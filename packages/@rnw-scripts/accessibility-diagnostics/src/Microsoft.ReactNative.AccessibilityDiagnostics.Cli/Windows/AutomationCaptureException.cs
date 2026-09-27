// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Cli.Windows;

internal sealed class AutomationCaptureException : Exception
{
    public AutomationCaptureException(string message)
        : base(message)
    {
    }

    public AutomationCaptureException(string message, Exception innerException)
        : base(message, innerException)
    {
    }
}
