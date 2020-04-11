// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// NYI:
//   implement multi source (parse out most suitable image source from array of
//   sources)
#include "pch.h"

#include "ImageViewManager.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <IReactInstance.h>
#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Views/ShadowNodeBase.h>
#include "ReactImage.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Controls;
} // namespace winrt

// Such code is better to move to a seperate parser layer
template <>
struct json_type_traits<react::uwp::ReactImageSource> {
  static react::uwp::ReactImageSource parseJson(const folly::dynamic &json) {
    react::uwp::ReactImageSource source;
    for (auto &item : json.items()) {
      if (item.first == "uri")
        source.uri = item.second.asString();
      else if (item.first == "method")
        source.method = item.second.asString();
      else if (item.first == "headers")
        source.headers = item.second;
      else if (item.first == "width")
        source.width = item.second.asDouble();
      else if (item.first == "height")
        source.height = item.second.asDouble();
      else if (item.first == "scale")
        source.scale = item.second.asDouble();
      else if (item.first == "__packager_asset")
        source.packagerAsset = item.second.asBool();
    }
    return source;
  }
};

template <>
struct json_type_traits<react::uwp::ResizeMode> {
  static react::uwp::ResizeMode parseJson(const folly::dynamic &json) {
    auto resizeMode{react::uwp::ResizeMode::Contain};

    if (json == "cover") {
      resizeMode = react::uwp::ResizeMode::Cover;
    } else if (json == "contain") {
      resizeMode = react::uwp::ResizeMode::Contain;
    } else if (json == "stretch") {
      resizeMode = react::uwp::ResizeMode::Stretch;
    } else if (json == "center") {
      resizeMode = react::uwp::ResizeMode::Center;
    } else if (json == "repeat") {
      resizeMode = react::uwp::ResizeMode::Repeat;
    }

    return resizeMode;
  }
};

namespace react {
namespace uwp {

class ImageShadowNode : public ShadowNodeBase {
 public:
  ImageShadowNode() = default;

  void createView() override {
    ShadowNodeBase::createView();
    auto reactImage{m_view.as<ReactImage>()};

    m_onLoadEndToken = reactImage->OnLoadEnd([imageViewManager{static_cast<ImageViewManager *>(GetViewManager())},
                                              reactImage](const auto &, const bool &succeeded) {
      ReactImageSource source{reactImage->Source()};

      imageViewManager->EmitImageEvent(reactImage.as<winrt::Grid>(), succeeded ? "topLoad" : "topError", source);
      imageViewManager->EmitImageEvent(reactImage.as<winrt::Grid>(), "topLoadEnd", source);
    });
  }

  void onDropViewInstance() override {
    auto reactImage{m_view.as<ReactImage>()};
    reactImage->OnLoadEnd(m_onLoadEndToken);
  }

 private:
  winrt::event_token m_onLoadEndToken;
};

ImageViewManager::ImageViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *ImageViewManager::GetName() const {
  return "RCTImageView";
}

XamlView ImageViewManager::CreateViewCore(int64_t /*tag*/) {
  return ReactImage::Create().as<winrt::Grid>();
}

facebook::react::ShadowNode *ImageViewManager::createShadow() const {
  return new ImageShadowNode();
}

bool ImageViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto grid{nodeToUpdate->GetView().as<winrt::Grid>()};

  if (grid == nullptr)
    return true;

  bool finalizeBorderRadius{false};
  bool ret = true;

  if (propertyName == "source") {
    setSource(grid, propertyValue);
  } else if (propertyName == "resizeMode") {
    auto resizeMode{json_type_traits<react::uwp::ResizeMode>::parseJson(propertyValue)};
    auto reactImage{grid.as<ReactImage>()};
    reactImage->ResizeMode(resizeMode);
  } else if (TryUpdateCornerRadiusOnNode(nodeToUpdate, grid, propertyName, propertyValue)) {
    finalizeBorderRadius = true;
  } else if (TryUpdateBorderProperties(nodeToUpdate, grid, propertyName, propertyValue)) {
  } else {
    ret = Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
    // TODO: overflow
  }

  if (finalizeBorderRadius)
    UpdateCornerRadiusOnElement(nodeToUpdate, grid);

  return ret;
}

void ImageViewManager::EmitImageEvent(winrt::Grid grid, const char *eventName, ReactImageSource &source) {
  auto reactInstance{m_wkReactInstance.lock()};
  if (reactInstance == nullptr)
    return;

  int64_t tag = grid.Tag().as<winrt::IPropertyValue>().GetInt64();
  folly::dynamic imageSource =
      folly::dynamic::object()("url", source.uri)("width", source.width)("height", source.height);

  folly::dynamic eventData = folly::dynamic::object()("target", tag)("source", imageSource);
  reactInstance->DispatchEvent(tag, eventName, std::move(eventData));
}

void ImageViewManager::setSource(winrt::Grid grid, const folly::dynamic &data) {
  auto instance{m_wkReactInstance.lock()};
  if (instance == nullptr)
    return;

  auto sources{json_type_traits<std::vector<ReactImageSource>>::parseJson(data)};
  sources[0].bundleRootPath = instance->GetBundleRootPath();

  if (sources[0].packagerAsset && sources[0].uri.find("file://") == 0) {
    sources[0].uri.replace(0, 7, sources[0].bundleRootPath);
  }

  auto reactImage{grid.as<ReactImage>()};

  EmitImageEvent(grid, "topLoadStart", sources[0]);
  reactImage->Source(sources[0]);
}

folly::dynamic ImageViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topLoadStart"] = folly::dynamic::object("registrationName", "onLoadStart");
  directEvents["topLoad"] = folly::dynamic::object("registrationName", "onLoad");
  directEvents["topLoadEnd"] = folly::dynamic::object("registrationName", "onLoadEnd");
  directEvents["topError"] = folly::dynamic::object("registrationName", "onError");

  return directEvents;
}

folly::dynamic ImageViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();
  // TODO: implement native props propagation from property map
  props.update(folly::dynamic::object("source", "Map")("resizeMode", "string")("accessibilityLabel", "string"));

  return props;
}
} // namespace uwp
} // namespace react
