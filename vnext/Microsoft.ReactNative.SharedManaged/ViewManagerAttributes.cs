// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{

  internal abstract class ViewManagerNamedAttribute : Attribute
  {
    public string Name { get; set; }

    public ViewManagerNamedAttribute() : base() { }

    public ViewManagerNamedAttribute(string name)
    {
      Name = name ?? throw new ArgumentNullException(nameof(name));
    }
  }

  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  internal class ViewManagerExportedViewConstantAttribute : ViewManagerNamedAttribute
  {
    public ViewManagerExportedViewConstantAttribute() : base() { }

    public ViewManagerExportedViewConstantAttribute(string name) : base(name) { }
  }

  [AttributeUsage(AttributeTargets.Method)]
  internal class ViewManagerPropertyAttribute : ViewManagerNamedAttribute
  {
    public ViewManagerPropertyType? Type { get; set; } = null;

    public ViewManagerPropertyAttribute() : base() { }

    public ViewManagerPropertyAttribute(string name) : base(name) { }

    public ViewManagerPropertyAttribute(string name, ViewManagerPropertyType type) : this(name)
    {
      Type = type;
    }
  }

  [AttributeUsage(AttributeTargets.Method)]
  internal class ViewManagerCommandAttribute : ViewManagerNamedAttribute
  {
    public long? CommandId { get; set; }

    public ViewManagerCommandAttribute() : base() { }

    public ViewManagerCommandAttribute(string name) : base(name) { }

    public ViewManagerCommandAttribute(string name, long commandId) : this(name)
    {
      CommandId = commandId;
    }
  }
}
