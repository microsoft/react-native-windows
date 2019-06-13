// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ViewManagerBase.h"

namespace react { namespace uwp {

class REACTWINDOWS_EXPORT FrameworkElementViewManager : public ViewManagerBase
{
  using Super = ViewManagerBase;
public:
  FrameworkElementViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  folly::dynamic GetNativeProps() const override;
  void UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap) override;

protected:
  virtual void TransferProperties(XamlView oldView, XamlView newView) override;

  void TransferProperty(XamlView oldView, XamlView newView, winrt::Windows::UI::Xaml::DependencyProperty dp);

  void AnnounceIfNeeded(winrt::FrameworkElement element);
};

} }
