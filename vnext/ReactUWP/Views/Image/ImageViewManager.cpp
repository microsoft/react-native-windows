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
#include <Views/ShadowNodeBase.h>
#include "ReactImage.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Controls;
} // namespace winrt

// Such code is better to move to a seperate parser layer
template <>
struct json_type_traits<react::uwp::ImageSource> {
  static react::uwp::ImageSource parseJson(const folly::dynamic &json) {
    react::uwp::ImageSource source;
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
      ImageSource source{reactImage->Source()};

      imageViewManager->EmitImageEvent(reactImage.as<winrt::Canvas>(), succeeded ? "topLoad" : "topError", source);
      imageViewManager->EmitImageEvent(reactImage.as<winrt::Canvas>(), "topLoadEnd", source);
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

XamlView ImageViewManager::CreateViewCore(int64_t tag) {
  return ReactImage::Create().as<winrt::Canvas>();
}

facebook::react::ShadowNode *ImageViewManager::createShadow() const {
  return new ImageShadowNode();
}

void ImageViewManager::UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) {
  auto canvas{nodeToUpdate->GetView().as<winrt::Canvas>()};

  if (canvas == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName{pair.first.getString()};
    const folly::dynamic &propertyValue{pair.second};

    if (propertyName == "source") {
      setSource(canvas, propertyValue);
    } else if (propertyName == "resizeMode") {
      auto resizeMode{json_type_traits<react::uwp::ResizeMode>::parseJson(propertyValue)};
      auto reactImage{canvas.as<ReactImage>()};
      reactImage->ResizeMode(resizeMode);
    }

    // TODO: overflow
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void ImageViewManager::EmitImageEvent(winrt::Canvas canvas, const char *eventName, ImageSource &source) {
  auto reactInstance{m_wkReactInstance.lock()};
  if (reactInstance == nullptr)
    return;

  int64_t tag = canvas.Tag().as<winrt::IPropertyValue>().GetInt64();
  folly::dynamic imageSource =
      folly::dynamic::object()("url", source.uri)("width", source.width)("height", source.height);

  folly::dynamic eventData = folly::dynamic::object()("target", tag)("source", imageSource);
  reactInstance->DispatchEvent(tag, eventName, std::move(eventData));
}

void ImageViewManager::setSource(winrt::Canvas canvas, const folly::dynamic &data) {
  auto instance{m_wkReactInstance.lock()};
  if (instance == nullptr)
    return;

  auto sources{json_type_traits<std::vector<ImageSource>>::parseJson(data)};
  sources[0].bundleRootPath = instance->GetBundleRootPath();

  auto reactImage{canvas.as<ReactImage>()};

  EmitImageEvent(canvas, "topLoadStart", sources[0]);
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
