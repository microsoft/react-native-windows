// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactConstantProvider
  {
    public ReactConstantProvider(IJSValueWriter writer)
    {
      Writer = writer;
    }

    public IJSValueWriter Writer { get; }

    public void Add<T>(string constantName, T value)
    {
      Writer.WriteObjectProperty(constantName, value);
    }
  }
}
