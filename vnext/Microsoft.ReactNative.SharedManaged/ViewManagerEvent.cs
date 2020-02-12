// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Windows.UI.Xaml;

namespace Microsoft.ReactNative.Managed
{
  internal delegate void ViewManagerEvent<TFrameworkElement, TEventData>(TFrameworkElement view, TEventData eventData) where TFrameworkElement : FrameworkElement;
}
