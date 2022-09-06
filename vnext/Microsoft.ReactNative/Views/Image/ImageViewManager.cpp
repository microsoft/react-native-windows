// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// NYI:
//   implement multi source (parse out most suitable image source from array of
//   sources)
#include "pch.h"

#include "ImageViewManager.h"

#include <UI.Xaml.Automation.h>
#include <winrt/Windows.Foundation.h>

#include <IReactInstance.h>
#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Views/DynamicAutomationProperties.h>
#include <Views/ShadowNodeBase.h>
#include "ReactImage.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace xaml::Controls;
using namespace xaml::Automation::Peers;
} // namespace winrt

// Such code is better to move to a seperate parser layer
template <>
struct json_type_traits<Microsoft::ReactNative::ReactImageSource> {
  static Microsoft::ReactNative::ReactImageSource parseJson(const winrt::Microsoft::ReactNative::JSValue &json) {
    Microsoft::ReactNative::ReactImageSource source;
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
struct json_type_traits<facebook::react::ImageResizeMode> {
  static facebook::react::ImageResizeMode parseJson(const winrt::Microsoft::ReactNative::JSValue &json) {
    auto resizeMode{facebook::react::ImageResizeMode::Contain};

    if (json == "cover") {
      resizeMode = facebook::react::ImageResizeMode::Cover;
    } else if (json == "contain") {
      resizeMode = facebook::react::ImageResizeMode::Contain;
    } else if (json == "stretch") {
      resizeMode = facebook::react::ImageResizeMode::Stretch;
    } else if (json == "center") {
      resizeMode = facebook::react::ImageResizeMode::Center;
    } else if (json == "repeat") {
      resizeMode = facebook::react::ImageResizeMode::Repeat;
    }

    return resizeMode;
  }
};

namespace Microsoft::ReactNative {

class ImageShadowNode : public ShadowNodeBase {
 public:
  ImageShadowNode() = default;

  void createView(const winrt::Microsoft::ReactNative::JSValueObject &props) override {
    ShadowNodeBase::createView(props);
    auto reactImage{m_view.as<ReactImage>()};

    // Image should default to "accessible: false", but this is not done on the JS level
    // https://reactnative.dev/docs/image#accessible
    xaml::Automation::AutomationProperties::SetAccessibilityView(*reactImage, winrt::AccessibilityView::Raw);
    DynamicAutomationProperties::SetAccessibilityRole(
        *reactImage, winrt::Microsoft::ReactNative::AccessibilityRoles::Image);

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

ImageViewManager::ImageViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ImageViewManager::GetName() const {
  return L"RCTImageView";
}

XamlView ImageViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
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
  auto reactImage{grid.as<ReactImage>()};

  bool finalizeBorderRadius{false};
  bool ret = true;

  if (propertyName == "source") {
    setSource(grid, propertyValue);
  } else if (propertyName == "resizeMode") {
    auto resizeMode{json_type_traits<facebook::react::ImageResizeMode>::parseJson(propertyValue)};
    reactImage->ResizeMode(resizeMode);
  } else if (
      propertyName == "blurRadius" &&
      (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
       propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)) {
    reactImage->BlurRadius(propertyValue.AsSingle());
  } else if (propertyName == "tintColor") {
    const auto isValidColorValue = IsValidColorValue(propertyValue);
    if (isValidColorValue || propertyValue.IsNull()) {
      const auto color = isValidColorValue ? SolidColorBrushFrom(propertyValue).Color() : winrt::Colors::Transparent();
      reactImage->TintColor(color);
    }
    // Override default accessibility behavior
  } else if (propertyName == "accessible" && propertyValue.IsNull()) {
    xaml::Automation::AutomationProperties::SetAccessibilityView(*reactImage, winrt::AccessibilityView::Raw);
  } else if (propertyName == "accessibilityRole" && propertyValue.IsNull()) {
    DynamicAutomationProperties::SetAccessibilityRole(
        *reactImage, winrt::Microsoft::ReactNative::AccessibilityRoles::Image);
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
  int64_t tag = GetTag(grid);
  folly::dynamic imageSource =
      folly::dynamic::object()("uri", source.uri)("width", source.width)("height", source.height);

  folly::dynamic eventData = folly::dynamic::object()("target", tag)("source", imageSource);
  GetReactContext().DispatchEvent(tag, eventName, std::move(eventData));
}

void ImageViewManager::setSource(winrt::Grid grid, const winrt::Microsoft::ReactNative::JSValue &data) {
  auto sources{json_type_traits<std::vector<ReactImageSource>>::parseJson(data)};
  sources[0].bundleRootPath = GetReactContext().SettingsSnapshot().BundleRootPath();

  if (sources[0].packagerAsset && sources[0].uri.find("file://") == 0) {
    sources[0].uri.replace(0, 7, sources[0].bundleRootPath);
  }

  auto reactImage{grid.as<ReactImage>()};

  EmitImageEvent(grid, "topLoadStart", sources[0]);
  reactImage->Source(sources[0]);
}

void ImageViewManager::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    const XamlView &viewToUpdate,
    float left,
    float top,
    float width,
    float height) {
  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
  if (viewToUpdate.ReadLocalValue(winrt::Grid::CornerRadiusProperty()) != xaml::DependencyProperty::UnsetValue()) {
    const auto maxCornerRadius = std::min(width, height) / 2;
    UpdateCornerRadiusOnElement(&nodeToUpdate, viewToUpdate.as<winrt::Grid>(), maxCornerRadius);
  }
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
