// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  struct ReactConstantProvider
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
