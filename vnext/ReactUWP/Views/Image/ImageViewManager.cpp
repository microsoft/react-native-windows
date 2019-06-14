// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// NYI:
//   implement image cache
//   implement prefetch functionality
//   implement multi source (parse out most suitable image source from array of sources)

#include "pch.h"

#include "ImageViewManager.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

#include <cxxreact/JsArgumentHelpers.h>

#include <IReactInstance.h>
#include <Views/ShadowNodeBase.h>
#include <Utils/PropertyHandlerUtils.h>
#include "ReactImage.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace Windows::Foundation;
  using namespace Windows::Storage::Streams;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Media::Imaging;
}

// Such code is better to move to a seperate parser layer
template<>
struct json_type_traits<react::uwp::ImageSource>
{
  static react::uwp::ImageSource parseJson(const folly::dynamic& json)
  {
    react::uwp::ImageSource source;
    for (auto& item : json.items())
    {
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

template<>
struct json_type_traits<react::uwp::ResizeMode>
{
  static react::uwp::ResizeMode parseJson(const folly::dynamic& json)
  {
    auto resizeMode = react::uwp::ResizeMode::Contain;

    if (json == "cover")
    {
      resizeMode = react::uwp::ResizeMode::Cover;
    }
    else if (json == "contain")
    {
      resizeMode = react::uwp::ResizeMode::Contain;
    }
    else if (json == "stretch")
    {
      resizeMode = react::uwp::ResizeMode::Stretch;
    }
    else if (json == "center")
    {
      resizeMode = react::uwp::ResizeMode::Center;
    }
    else if (json == "repeat")
    {
      resizeMode = react::uwp::ResizeMode::Repeat;
    }

    return resizeMode;
  }
};

namespace react { namespace uwp {

  ImageViewManager::ImageViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
    : Super(reactInstance)
  {
  }

  const char* ImageViewManager::GetName() const
  {
    return "RCTImageView";
  }

  XamlView ImageViewManager::CreateViewCore(int64_t tag)
  {
    auto reactImage{ ReactImage::Create() };

    reactImage->OnLoadEnd([=](const auto&, const bool& succeeded)
    {
        ImageSource source{ reactImage->Source() };

        EmitImageEvent(m_wkReactInstance.lock(), reactImage.as<winrt::Canvas>(), succeeded ? "topLoad" : "topError", source);
        EmitImageEvent(m_wkReactInstance.lock(), reactImage.as<winrt::Canvas>(), "topLoadEnd", source);
    });

    return reactImage.as<winrt::Canvas>();
  }

  void ImageViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
  {
    auto canvas = nodeToUpdate->GetView().as<winrt::Canvas>();

    if (canvas == nullptr)
      return;

    for (const auto& pair : reactDiffMap.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;

      if (propertyName == "source")
      {
        setSource(canvas, propertyValue);
      }
      else if (propertyName == "resizeMode")
      {
        auto resizeMode = json_type_traits<react::uwp::ResizeMode>::parseJson(propertyValue);
        auto reactImage{ canvas.as<ReactImage>() };
        reactImage->ResizeMode(resizeMode);
      }

      // TODO: overflow
    }

    Super::UpdateProperties(nodeToUpdate, reactDiffMap);
  }

  void EmitImageEvent(const std::shared_ptr<react::uwp::IReactInstance>& reactInstance, winrt::Canvas canvas, const char* eventName, ImageSource& source)
  {
    if (reactInstance == nullptr)
      return;

    int64_t tag = canvas.Tag().as<winrt::IPropertyValue>().GetInt64();
    folly::dynamic imageSource = folly::dynamic::object()
      ("url", source.uri)
      ("width", source.width)
      ("height", source.height);

    folly::dynamic eventData = folly::dynamic::object()
      ("target", tag)
      ("source", imageSource);
    reactInstance->DispatchEvent(tag, eventName, std::move(eventData));
  }

  void ImageViewManager::setSource(winrt::Canvas canvas, const folly::dynamic& data)
  {
    auto instance = m_wkReactInstance.lock();
    if (instance == nullptr)
      return;

    auto sources = json_type_traits<std::vector<ImageSource>>::parseJson(data);

    auto uriString = sources[0].uri;
    if (uriString.length() == 0)
    {
      EmitImageEvent(instance, canvas, "topError", sources[0]);
      return;
    }

    auto reactImage{ canvas.as<ReactImage>() };
    EmitImageEvent(instance, canvas, "topLoadStart", sources[0]);
    reactImage->Source(sources[0]);
  }

  folly::dynamic ImageViewManager::GetExportedCustomDirectEventTypeConstants() const
  {
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
    props.update(folly::dynamic::object
      ("source", "Map")
      ("resizeMode", "string")
      ("accessibilityLabel", "string")
    );

    return props;
  }


//
// ImageViewManagerModule::ImageViewManagerModuleImpl
//
class ImageViewManagerModule::ImageViewManagerModuleImpl
{
public:
  ImageViewManagerModuleImpl(ImageViewManagerModule* parent, const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread)
    : m_parent(parent)
    , m_queueThread(defaultQueueThread)
  { }

  void Disconnect()
  {
    m_parent = nullptr;
  }

  void getSize(std::string uri, Callback successCallback, Callback errorCallback);
  void prefetchImage(std::string uri, Callback successCallback, Callback errorCallback);
  void queryCache(const folly::dynamic& requests, Callback successCallback, Callback errorCallback);

private:
  ImageViewManagerModule *m_parent;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
};

winrt::fire_and_forget GetImageSizeAsync(std::string uri, facebook::xplat::module::CxxModule::Callback successCallback, facebook::xplat::module::CxxModule::Callback errorCallback)
{
  bool succeeded{ false };

  try
  {
    ImageSource source;
    source.uri = uri;

    winrt::InMemoryRandomAccessStream memoryStream{ co_await react::uwp::GetImageStreamAsync(source) };

    if (!memoryStream)
    {
      EmitImageEvent(std::weak_ptr<IReactInstance>().lock(), {}, "topError", source);
    }

    winrt::BitmapImage bitmap;
    co_await bitmap.SetSourceAsync(memoryStream);

    if (bitmap)
    {
      successCallback({ bitmap.PixelWidth(), bitmap.PixelHeight() });
      succeeded = true;
    }
  }
  catch (winrt::hresult_error const &)
  {
  }

  if (!succeeded)
    errorCallback({});

  co_return;
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::getSize(std::string uri, Callback successCallback, Callback errorCallback)
{
  GetImageSizeAsync(uri, successCallback, errorCallback);
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::prefetchImage(std::string /*uri*/, Callback successCallback, Callback /*errorCallback*/)
{
  // NotYetImplemented
  successCallback({});
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::queryCache(const folly::dynamic& requests, Callback successCallback, Callback /*errorCallback*/)
{
  // NotYetImplemented
  successCallback({ folly::dynamic::object() });
}


//
// ImageViewManagerModule
//
const char* ImageViewManagerModule::name = "ImageViewManager";

ImageViewManagerModule::ImageViewManagerModule(const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread)
  : m_imageViewManagerModule(std::make_shared<ImageViewManagerModuleImpl>(this, defaultQueueThread))
{
}

ImageViewManagerModule::~ImageViewManagerModule()
{
}

std::string ImageViewManagerModule::getName()
{
  return name;
}

std::map<std::string, folly::dynamic> ImageViewManagerModule::getConstants()
{
  return { {} };
}

auto ImageViewManagerModule::getMethods() -> std::vector<Method>
{
  std::shared_ptr<ImageViewManagerModuleImpl> imageViewManager(m_imageViewManagerModule);
  return {
    Method("getSize", [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback)
    {
      imageViewManager->getSize(facebook::xplat::jsArgAsString(args, 0), successCallback, errorCallback);
    }, AsyncTag),
    Method("prefetchImage", [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback)
    {
      imageViewManager->prefetchImage(facebook::xplat::jsArgAsString(args, 0), successCallback, errorCallback);
    }),
    Method("queryCache", [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback)
    {
      imageViewManager->queryCache(args[0], successCallback, errorCallback);
    }),
  };
}

} }
