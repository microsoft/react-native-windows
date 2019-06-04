// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// NYI:
//   implement image cache
//   implement prefetch functionality
//   implement multi source (parse out most suitable image source from array of sources)

#include "pch.h"

#include "ImageViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>
#include <Utils/PropertyHandlerUtils.h>

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>

#include "unicode.h"

#include <condition_variable>
#include <future>
#include <mutex>
#include <type_traits>

#include "Utils/PropertyHandlerUtils.h"

#include <cxxreact/JsArgumentHelpers.h>

#include "BorderEffect.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace Microsoft::UI::Composition::Effects;
  using namespace Windows::Foundation;
  using namespace Windows::Storage::Streams;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Media;
  using namespace Windows::UI::Xaml::Media::Imaging;
  using namespace Windows::Web::Http;

}

namespace react {
  namespace uwp {
    struct ImageSource
    {
      std::string uri;
      std::string method;
      folly::dynamic headers;
      double width = 0;
      double height = 0;
      double scale = 1.0;
      bool packagerAsset = false;
    };
} }

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
struct json_type_traits<winrt::Stretch>
{
  static winrt::Stretch parseJson(const folly::dynamic& json)
  {
    winrt::Stretch stretch;
    if (json == "cover")
    {
      stretch = winrt::Stretch::UniformToFill;
    }
    else if (json == "contain")
    {
      stretch = winrt::Stretch::Uniform;
    }
    else
    {
      stretch = winrt::Stretch::Fill;
    }

    return stretch;
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
    winrt::Canvas canvas;
    return canvas;
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
        auto stretch = json_type_traits<winrt::Stretch>::parseJson(propertyValue);
        // image.Stretch(stretch);
      }

      // TODO: overflow
    }

    Super::UpdateProperties(nodeToUpdate, reactDiffMap);
  }

  void EmitImageEvent(const std::shared_ptr<react::uwp::IReactInstance> &reactInstance, winrt::Canvas canvas, const char* eventName, ImageSource& source)
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

  std::future<void> DownloadImageAsync(winrt::Canvas canvas, ImageSource source, std::weak_ptr<IReactInstance> instanceWeak)
  {
    // Since this is a fire and forget function (not waited upon when called), we need to wrap in a
    // try/catch or uncaught exceptions will take down the process
    try
    {
      winrt::HttpClient httpClient;
      winrt::HttpMethod httpMethod(nullptr);

      if (source.method.empty())
        httpMethod = winrt::HttpMethod::Get();
      else
        httpMethod = winrt::HttpMethod(facebook::react::unicode::utf8ToUtf16(source.method));

      winrt::Uri uri(facebook::react::unicode::utf8ToUtf16(source.uri));

      winrt::HttpRequestMessage request(httpMethod, uri);

      if (!source.headers.empty())
      {
        for (auto& header : source.headers.items())
        {
          auto& name = header.first.getString();
          auto& value = header.second.getString();

          if (_stricmp(name.c_str(), "authorization") == 0)
            request.Headers().TryAppendWithoutValidation(facebook::react::unicode::utf8ToUtf16(name), facebook::react::unicode::utf8ToUtf16(value));
          else
            request.Headers().Append(facebook::react::unicode::utf8ToUtf16(name), facebook::react::unicode::utf8ToUtf16(value));
        }
      }

      winrt::HttpResponseMessage response = co_await httpClient.SendRequestAsync(request);

      auto instance = instanceWeak.lock();
      if (response.StatusCode() == winrt::HttpStatusCode::Ok)
      {
        winrt::IInputStream inputStream = co_await response.Content().ReadAsInputStreamAsync();
        winrt::InMemoryRandomAccessStream memoryStream;
        co_await winrt::RandomAccessStream::CopyAsync(inputStream, memoryStream);
        memoryStream.Seek(0);

        // winrt::LoadedImageSurface().StartLoadFromStream(memoryStream);

        winrt::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(memoryStream);

        winrt::ImageBrush brush;
        brush.ImageSource(bitmap);

        canvas.Background(brush);

        EmitImageEvent(instance, canvas, "topLoad", source);
      }
      else
      {
        EmitImageEvent(instance, canvas, "topError", source);
      }
    }
    catch (winrt::hresult_error const &)
    {
      EmitImageEvent(instanceWeak.lock(), canvas, "topError", source);
    }

    EmitImageEvent(instanceWeak.lock(), canvas, "topLoadEnd", source);
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

    if (sources[0].packagerAsset && uriString.find("file://") == 0)
      uriString.replace(0, 7, "ms-appx:///Bundle/");

    bool needsDownload = false;

    try
    {
      auto uri = winrt::Uri(winrt::hstring(asWStr(uriString).c_str()));
      auto scheme = uri.SchemeName();
      needsDownload = scheme == L"http" || scheme == L"https";

      EmitImageEvent(instance, canvas, "topLoadStart", sources[0]);
      if (needsDownload)
      {
        // FUTURE: This should get a weak_ptr from instance
        // fix when the win32 instance merge happens
        DownloadImageAsync(canvas, sources[0], instance);
      }
      else
      {
        winrt::BitmapImage bitmap;
        bitmap.UriSource(uri);

        winrt::ImageBrush brush;
        brush.ImageSource(bitmap);

        canvas.Background(brush);

        EmitImageEvent(instance, canvas, "topLoad", sources[0]);
      }
    }
    catch (...)
    {
      OutputDebugString(L"caught exception setting up image source");
      EmitImageEvent(instance, canvas, "topError", sources[0]);
    }

    if (!needsDownload)
      EmitImageEvent(instance, canvas, "topLoadEnd", sources[0]);
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
  winrt::Canvas canvas;
  ImageSource source;
  source.uri = uri;

  winrt::apartment_context ui_thread;

  bool succeeded = false;
  try
  {
    co_await DownloadImageAsync(canvas, source, std::weak_ptr<IReactInstance>());

    // Get back to the UI-thread to read properties on Image
    co_await ui_thread;
    if (auto brush = canvas.Background().try_as<winrt::ImageBrush>())
    {
      if (auto bitmap = brush.ImageSource().try_as<winrt::BitmapImage>())
      {
        successCallback({ bitmap.PixelWidth(), bitmap.PixelHeight() });
        succeeded = true;
      }
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
