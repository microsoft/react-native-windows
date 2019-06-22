// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <Views/FrameworkElementViewManager.h>
#include "ReactImage.h"

namespace react { namespace uwp {

  class ImageViewManager : public FrameworkElementViewManager
  {
    using Super = FrameworkElementViewManager;
  public:
    ImageViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

    const char* GetName() const override;
    void UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap) override;

    folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
    folly::dynamic GetNativeProps() const override;
    void EmitImageEvent(winrt::Windows::UI::Xaml::Controls::Canvas canvas, const char* eventName, ImageSource& source);

  protected:
    XamlView CreateViewCore(int64_t tag) override;

  private:
    void setSource(winrt::Windows::UI::Xaml::Controls::Canvas canvas, const folly::dynamic& sources);
  };
} }
