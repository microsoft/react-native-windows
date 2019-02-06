// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace react { namespace uwp {

class TextInputViewManager : public FrameworkElementViewManager
{
  using Super = FrameworkElementViewManager;
public:
  TextInputViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  const char* GetName() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap) override;
  void DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs) override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

protected:
  XamlView CreateViewCore(int64_t tag) override;

private:
  bool m_shouldClearTextOnFocus = false;
  bool m_shouldSelectTextOnFocus = false;
};

} }
