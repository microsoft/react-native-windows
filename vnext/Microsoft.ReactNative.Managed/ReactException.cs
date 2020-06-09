// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  public class ReactException : Exception
  {
    public ReactException()
    {
    }

    public ReactException(string message)
        : base(message)
    {
    }

    public ReactException(string message, Exception inner)
        : base(message, inner)
    {
    }
  }
}
