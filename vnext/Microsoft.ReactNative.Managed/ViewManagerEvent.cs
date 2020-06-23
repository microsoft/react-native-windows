// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#if !USE_WINUI3
using Windows.UI.Xaml;
#else
using Microsoft.UI.Xaml;
#endif

namespace Microsoft.ReactNative.Managed
{
  public delegate void ViewManagerEvent<TFrameworkElement, TEventData>(TFrameworkElement view, TEventData eventData) where TFrameworkElement : FrameworkElement;
}
