// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

using Windows.UI.Xaml;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  internal abstract class AttributedViewManagerWithChildren<TFrameworkElement> :
    AttributedViewManager<TFrameworkElement>,
    IViewManagerWithChildren
    where TFrameworkElement : FrameworkElement, new()
  {
    protected AttributedViewManagerWithChildren(IReactContext reactContext) : base(reactContext) { }

    #region Children

    public virtual IList<UIElement> GetChildren(TFrameworkElement parent) => throw new NotImplementedException();

    public virtual void AddView(FrameworkElement parent, UIElement child, long index)
    {
      if (parent is TFrameworkElement parentAsT)
      {
        GetChildren(parentAsT).Insert((int)index, child);
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(parent));
      }
    }

    public virtual void RemoveAllChildren(FrameworkElement parent)
    {
      if (parent is TFrameworkElement parentAsT)
      {
        GetChildren(parentAsT).Clear();
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(parent));
      }
    }

    public virtual void RemoveChildAt(FrameworkElement parent, long index)
    {
      if (parent is TFrameworkElement parentAsT)
      {
        GetChildren(parentAsT).RemoveAt((int)index);
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(parent));
      }
    }

    public virtual void ReplaceChild(FrameworkElement parent, UIElement oldChild, UIElement newChild)
    {
      if (parent is TFrameworkElement parentAsT)
      {
        IList<UIElement> children = GetChildren(parentAsT);

        int index = children.IndexOf(oldChild);
        children.RemoveAt(index);
        children.Insert(index, newChild);
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(parent));
      }
    }

    #endregion
  }
}
