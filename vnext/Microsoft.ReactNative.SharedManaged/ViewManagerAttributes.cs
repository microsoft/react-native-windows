// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{

  internal abstract class ViewManagerNamedAttribute : Attribute
  {
    public string Name { get; private set; }

    public ViewManagerNamedAttribute(string name)
    {
      Name = name ?? throw new ArgumentNullException(nameof(name));
    }
  }
  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  internal class ViewManagerExportedViewConstantAttribute : ViewManagerNamedAttribute
  {
    public ViewManagerExportedViewConstantAttribute(string name) : base(name)
    {
    }
  }

  [AttributeUsage(AttributeTargets.Method)]
  internal class ViewManagerPropertySetterAttribute : ViewManagerNamedAttribute
  {
    public ViewManagerPropertyType? Type { get; set; } = null;

    public ViewManagerPropertySetterAttribute(string name) : base(name)
    {
    }

    public ViewManagerPropertySetterAttribute(string name, ViewManagerPropertyType type) : this(name)
    {
      Type = type;
    }
  }

  [AttributeUsage(AttributeTargets.Method)]
  internal class ViewManagerCommandAttribute : Attribute
  {
    public string Name { get; set; }

    public long? CommandId { get; set; }

    public ViewManagerCommandAttribute() : base() { }

    public ViewManagerCommandAttribute(string name) : base()
    {
      Name = name;
    }

    public ViewManagerCommandAttribute(string name, long commandId) : this(name)
    {
      CommandId = commandId;
    }
  }
}
