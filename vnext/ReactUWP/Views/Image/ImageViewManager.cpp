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

// ImageShadowNode
void ImageShadowNode::createView() {
  ShadowNodeBase::createView();
  auto reactImage{m_view.as<ReactImage>()};

  m_onLoadEndToken = reactImage->OnLoadEnd(
      [this, reactImage](const auto &, const bool &succeeded) {
        ImageSource source{reactImage->Source()};

        EmitImageEvent(succeeded ? "topLoad" : "topError", source);
        EmitImageEvent("topLoadEnd", source);
      });
}

void ImageShadowNode::onDropViewInstance() {
  auto reactImage{m_view.as<ReactImage>()};
  reactImage->OnLoadEnd(m_onLoadEndToken);
}

void ImageShadowNode::updateProperties(const folly::dynamic &&props) {
  if (auto reactImage{m_view.as<ReactImage>()}) {
    updateMaxSize(props);

    for (const auto &pair : props.items()) {
      const std::string &propertyName{pair.first.getString()};
      const folly::dynamic &propertyValue{pair.second};

      if (propertyName == "source") {
        setSource(propertyValue, m_maxSize);
      } else if (propertyName == "resizeMode") {
        auto resizeMode{
            json_type_traits<react::uwp::ResizeMode>::parseJson(propertyValue)};
        reactImage->ResizeMode(resizeMode);
      }

      // TODO: overflow

      GetViewManager()->UpdateProperties(this, props);
    }
  }
}

void ImageShadowNode::EmitImageEvent(
    const char *eventName,
    ImageSource &source) {
  if (auto instance{GetViewManager()->GetReactInstance().lock()}) {
    auto canvas{m_view.as<winrt::Canvas>()};
    int64_t tag = canvas.Tag().as<winrt::IPropertyValue>().GetInt64();

    folly::dynamic imageSource = folly::dynamic::object()("url", source.uri)(
        "width", source.width)("height", source.height);

    folly::dynamic eventData =
        folly::dynamic::object()("target", tag)("source", imageSource);

    instance->DispatchEvent(tag, eventName, std::move(eventData));
  }
}

void ImageShadowNode::setSource(
    const folly::dynamic &data,
    const winrt::Size &maxSize) {
  if (auto instance{GetViewManager()->GetReactInstance().lock()}) {
    auto sources{json_type_traits<std::vector<ImageSource>>::parseJson(data)};
    sources[0].bundleRootPath = instance->GetBundleRootPath();

    auto reactImage{m_view.as<ReactImage>()};

    EmitImageEvent("topLoadStart", sources[0]);
    reactImage->Source(sources[0], maxSize);
  }
}

void ImageShadowNode::updateMaxSize(const folly::dynamic &props) {
  float width = tryGetPropAsFloat(props, "maxWidth");
  if (width == 0) {
    width = tryGetPropAsFloat(props, "width");
  }
  if (width != 0) {
    m_maxSize.Width = width;
  }

  float height = tryGetPropAsFloat(props, "maxHeight");
  if (height == 0) {
    height = tryGetPropAsFloat(props, "height");
  }
  if (height != 0) {
    m_maxSize.Height = height;
  }
}

float ImageShadowNode::tryGetPropAsFloat(
    const folly::dynamic &props,
    const char *propName) {
  if (props.find(propName) != props.items().end() &&
      props[propName].isNumber()) {
    return folly::to<float>(props[propName].asDouble());
  }

  return 0;
}

// ImageViewManager
ImageViewManager::ImageViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *ImageViewManager::GetName() const {
  return "RCTImageView";
}

XamlView ImageViewManager::CreateViewCore(int64_t tag) {
  return ReactImage::Create().as<winrt::Canvas>();
}

facebook::react::ShadowNode *ImageViewManager::createShadow() const {
  return new ImageShadowNode();
}

folly::dynamic ImageViewManager::GetExportedCustomDirectEventTypeConstants()
    const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topLoadStart"] =
      folly::dynamic::object("registrationName", "onLoadStart");
  directEvents["topLoad"] =
      folly::dynamic::object("registrationName", "onLoad");
  directEvents["topLoadEnd"] =
      folly::dynamic::object("registrationName", "onLoadEnd");
  directEvents["topError"] =
      folly::dynamic::object("registrationName", "onError");

  return directEvents;
}

folly::dynamic ImageViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();
  // TODO: implement native props propagation from property map
  props.update(folly::dynamic::object("source", "Map")("resizeMode", "string")(
      "accessibilityLabel", "string"));

  return props;
}
} // namespace uwp
} // namespace react
