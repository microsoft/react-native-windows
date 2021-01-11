// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// NYI:
//   implement multi source (parse out most suitable image source from array of
//   sources)
#include "pch.h"

#include "ImageViewManager.h"

#include <winrt/Windows.Foundation.h>

#include <IReactInstance.h>
#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Views/ShadowNodeBase.h>
#include "ReactImage.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace xaml::Controls;
} // namespace winrt

// Such code is better to move to a seperate parser layer
template <>
struct json_type_traits<react::uwp::ReactImageSource> {
  static react::uwp::ReactImageSource parseJson(const winrt::Microsoft::ReactNative::JSValue &json) {
    react::uwp::ReactImageSource source;
    for (auto &item : json.AsObject()) {
      if (item.first == "uri")
        source.uri = item.second.AsString();
      else if (item.first == "method")
        source.method = item.second.AsString();
      else if (item.first == "headers") {
        for (auto &header : item.second.AsObject()) {
          source.headers.push_back(std::make_pair(header.first, header.second.AsString()));
        }
      } else if (item.first == "width")
        source.width = item.second.AsDouble();
      else if (item.first == "height")
        source.height = item.second.AsDouble();
      else if (item.first == "scale")
        source.scale = item.second.AsDouble();
      else if (item.first == "__packager_asset")
        source.packagerAsset = item.second.AsBoolean();
    }
    return source;
  }
};

template <>
struct json_type_traits<react::uwp::ResizeMode> {
  static react::uwp::ResizeMode parseJson(const winrt::Microsoft::ReactNative::JSValue &json) {
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

namespace Microsoft::ReactNative {

using ReactImage = react::uwp::ReactImage;

class ImageShadowNode : public ShadowNodeBase {
 public:
  ImageShadowNode() = default;

  void createView() override {
    ShadowNodeBase::createView();
    auto reactImage{m_view.as<ReactImage>()};

    m_onLoadEndToken = reactImage->OnLoadEnd([imageViewManager{static_cast<ImageViewManager *>(GetViewManager())},
                                              reactImage](const auto &, const bool &succeeded) {
      react::uwp::ReactImageSource source{reactImage->Source()};

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

ImageViewManager::ImageViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ImageViewManager::GetName() const {
  return L"RCTImageView";
}

XamlView ImageViewManager::CreateViewCore(int64_t /*tag*/) {
  return ReactImage::Create().as<winrt::Grid>();
}

ShadowNode *ImageViewManager::createShadow() const {
  return new ImageShadowNode();
}

bool ImageViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
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
  } else if (
      propertyName == "blurRadius" &&
      (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
       propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)) {
    auto reactImage{grid.as<ReactImage>()};
    reactImage->BlurRadius(propertyValue.AsSingle());
  } else if (propertyName == "tintColor" && react::uwp::IsValidColorValue(propertyValue)) {
    auto reactImage{grid.as<ReactImage>()};
    reactImage->TintColor(react::uwp::ColorFrom(propertyValue));
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

void ImageViewManager::EmitImageEvent(winrt::Grid grid, const char *eventName, react::uwp::ReactImageSource &source) {
  int64_t tag = grid.Tag().as<winrt::IPropertyValue>().GetInt64();
  folly::dynamic imageSource =
      folly::dynamic::object()("uri", source.uri)("width", source.width)("height", source.height);

  folly::dynamic eventData = folly::dynamic::object()("target", tag)("source", imageSource);
  GetReactContext().DispatchEvent(tag, eventName, std::move(eventData));
}

void ImageViewManager::setSource(winrt::Grid grid, const winrt::Microsoft::ReactNative::JSValue &data) {
  auto sources{json_type_traits<std::vector<react::uwp::ReactImageSource>>::parseJson(data)};
  sources[0].bundleRootPath = GetReactContext().SettingsSnapshot().BundleRootPath();

  if (sources[0].packagerAsset && sources[0].uri.find("file://") == 0) {
    sources[0].uri.replace(0, 7, sources[0].bundleRootPath);
  }

  auto reactImage{grid.as<ReactImage>()};

  EmitImageEvent(grid, "topLoadStart", sources[0]);
  reactImage->Source(sources[0]);
}

void ImageViewManager::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomDirectEventTypeConstants(writer);

  const PCWSTR eventNames[] = {// Generic events
                               L"LoadStart",
                               L"Load",
                               L"LoadEnd",
                               L"Error"};

  for (auto &eventBaseName : eventNames) {
    using namespace std::string_literals;
    std::wstring eventName = L"top"s + eventBaseName;
    std::wstring bubbleName = L"on"s + eventBaseName;

    writer.WritePropertyName(L"top"s + eventBaseName);
    writer.WriteObjectBegin();
    writer.WritePropertyName(L"registrationName");
    writer.WriteString(L"on"s + eventBaseName);
    writer.WriteObjectEnd();
  }
}

void ImageViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
  // TODO: implement native props propagation from property map

  writer.WritePropertyName(L"source");
  writer.WriteString(L"Map");
  writer.WritePropertyName(L"resizeMode");
  writer.WriteString(L"string");
  writer.WritePropertyName(L"accessibilityLabel");
  writer.WriteString(L"string");
  writer.WritePropertyName(L"blurRadius");
  writer.WriteString(L"number");
}
} // namespace Microsoft::ReactNative
