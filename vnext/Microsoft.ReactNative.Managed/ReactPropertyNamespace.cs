// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactPropertyNamespace
  {
    public IReactPropertyNamespace Handle { get; }

    public bool IsValid => Handle != null;

    public string NamespaceName => IsValid ? Handle.NamespaceName : null;

    public static explicit operator bool(ReactPropertyNamespace propertyNamespace) => propertyNamespace.IsValid;

    public ReactPropertyNamespace(IReactPropertyNamespace handle = null)
    {
      Handle = handle;
    }

    public ReactPropertyNamespace(string namespaceName)
    {
      Handle = ReactPropertyBagHelper.GetNamespace(namespaceName);
    }
  }
}
