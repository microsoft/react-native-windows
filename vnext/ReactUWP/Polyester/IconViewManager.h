// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react { namespace uwp { namespace polyester {

class IconViewManager : public FrameworkElementViewManager
{
  using Super = FrameworkElementViewManager;
public:
  IconViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  const char* GetName() const override;
  folly::dynamic GetNativeProps() const override;

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap) override;

protected:
  XamlView CreateViewCore(int64_t tag) override;

private:
  static void UpdateFontColorProps(winrt::Windows::UI::Xaml::Documents::Glyphs glyphs);
};

} } }
