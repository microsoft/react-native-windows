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
#include <Views/ShadowNodeBase.h>
#include "DynamicAutomationProperties.h"
#include "ReactImage.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace xaml::Controls;
using namespace xaml::Automation::Peers;
} // namespace winrt

// Such code is better to move to a seperate parser layer
template <>
struct json_type_traits<Microsoft::ReactNative::ReactImageSourceWithHeaders> {
  static Microsoft::ReactNative::ReactImageSourceWithHeaders parseJson(
      const winrt::Microsoft::ReactNative::JSValue &json) {
    Microsoft::ReactNative::ReactImageSourceWithHeaders source;
    for (auto &item : json.AsObject()) {
      if (item.first == "uri")
        source.Uri = winrt::to_hstring(item.second.AsString());
      else if (item.first == "method")
        source.Method = winrt::to_hstring(item.second.AsString());
      else if (item.first == "headers") {
        for (auto &header : item.second.AsObject()) {
          source.headers.push_back(std::make_pair(header.first, header.second.AsString()));
        }
      } else if (item.first == "width")
        source.Width = item.second.AsDouble();
      else if (item.first == "height")
        source.Height = item.second.AsDouble();
      else if (item.first == "scale")
        source.Scale = item.second.AsDouble();
      else if (item.first == "__packager_asset")
        source.PackagerAsset = item.second.AsBoolean();
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
      ReactImageSourceWithHeaders source{reactImage->Source()};

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
      const auto color = isValidColorValue ? ColorFrom(propertyValue) : winrt::Colors::Transparent();
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

void ImageViewManager::EmitImageEvent(
    winrt::Grid grid,
    const char *eventName,
    const winrt::Microsoft::ReactNative::ReactImageSource &source) {
  int64_t tag = GetTag(grid);
  folly::dynamic imageSource =
      folly::dynamic::object()("uri", winrt::to_string(source.Uri))("width", source.Width)("height", source.Height);

  folly::dynamic eventData = folly::dynamic::object()("target", tag)("source", imageSource);
  GetReactContext().DispatchEvent(tag, eventName, std::move(eventData));
}

void ImageViewManager::setSource(winrt::Grid grid, const winrt::Microsoft::ReactNative::JSValue &data) {
  auto sources{json_type_traits<std::vector<ReactImageSourceWithHeaders>>::parseJson(data)};
  sources[0].BundleRootPath = winrt::to_hstring(GetReactContext().SettingsSnapshot().BundleRootPath());

  std::wstring uri{sources[0].Uri.data()};
  if (sources[0].PackagerAsset && uri.find(L"file://") == 0) {
    uri.replace(0, 7, sources[0].BundleRootPath);
    sources[0].Uri = uri;
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
