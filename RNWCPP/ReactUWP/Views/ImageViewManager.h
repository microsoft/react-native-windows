// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <cxxreact/CxxModule.h>
#include <Views/FrameworkElementViewManager.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt::Windows::UI::Xaml::Media { enum class Stretch; }

namespace facebook { namespace react {
  class MessageQueueThread;
} }

namespace react { namespace uwp {

  class ImageViewManager : public FrameworkElementViewManager
  {
    using Super = FrameworkElementViewManager;
  public:
    ImageViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

    const char* GetName() const override;
    void UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap) override;

    folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
    folly::dynamic GetNativeProps() const override;

  protected:
    XamlView CreateViewCore(int64_t tag) override;

  private:
    void setSource(winrt::Windows::UI::Xaml::Controls::Image image, folly::dynamic& sources);
  };

  class ImageViewManagerModule : public facebook::xplat::module::CxxModule
  {
  public:
    ImageViewManagerModule(const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread);
    virtual ~ImageViewManagerModule();

    // CxxModule
    std::string getName() override;
    std::map<std::string, folly::dynamic> getConstants() override;
    auto getMethods() -> std::vector<Method> override;

    static const char* name;

  private:
    class ImageViewManagerModuleImpl;
    std::shared_ptr<ImageViewManagerModuleImpl> m_imageViewManagerModule;
  };

} }
