// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core;

public sealed class SnapshotFormatException : Exception
{
    public SnapshotFormatException(string message)
        : base(message)
    {
    }

    public SnapshotFormatException(string message, Exception innerException)
        : base(message, innerException)
    {
    }
}
