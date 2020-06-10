// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public interface IJSValueTreeReader
  {
    JSValue Current { get; }

    JSValue Root { get; }
  }
}
