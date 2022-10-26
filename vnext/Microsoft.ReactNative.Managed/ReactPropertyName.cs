// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactPropertyName
  {
    public ReactPropertyNamespace Namespace => new ReactPropertyNamespace(IsValid ? Handle.Namespace : null);

    public string NamespaceName => IsValid ? Handle.Namespace.NamespaceName : null;

    public string LocalName => IsValid ? Handle.LocalName : null;

    public IReactPropertyName Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactPropertyName propertyName) => propertyName.IsValid;

    public ReactPropertyName(IReactPropertyName handle = null)
    {
      Handle = handle;
    }

    public ReactPropertyName(string localName)
      : this((IReactPropertyNamespace)null, localName) { }

    public ReactPropertyName(string namespaceName, string localName)
      : this(ReactPropertyBagHelper.GetNamespace(namespaceName), localName) { }

    public ReactPropertyName(ReactPropertyNamespace ns, string localName)
      : this(ns.Handle, localName) { }

    public ReactPropertyName(IReactPropertyNamespace ns, string localName)
    {
      Handle = ReactPropertyBagHelper.GetName(ns, localName);
    }
  }
}
