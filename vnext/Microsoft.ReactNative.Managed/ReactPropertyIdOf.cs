// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  public struct ReactPropertyId<T>
  {
    public ReactPropertyName PropertyName { get; }

    public ReactPropertyNamespace Namespace => PropertyName.Namespace;

    public string NamespaceName => PropertyName.NamespaceName;

    public string LocalName => PropertyName.LocalName;

    public IReactPropertyName Handle => PropertyName.Handle;

    public bool IsValid => PropertyName.IsValid;

    public static explicit operator bool(ReactPropertyId<T> propertyId) => propertyId.IsValid;

    public ReactPropertyId(ReactPropertyName propertyName)
    {
      PropertyName = propertyName;
    }

    public ReactPropertyId(IReactPropertyName handle = null)
      : this(new ReactPropertyName(handle)) { }

    public ReactPropertyId(string localName)
      : this(new ReactPropertyName(localName)) { }

    public ReactPropertyId(string namespaceName, string localName)
      : this(new ReactPropertyName(namespaceName, localName)) { }

    public ReactPropertyId(ReactPropertyNamespace ns, string localName)
  : this(new ReactPropertyName(ns, localName)) { }

    public ReactPropertyId(IReactPropertyNamespace ns, string localName)
      : this(new ReactPropertyName(ns, localName)) { }
  }
}
