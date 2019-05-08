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

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
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
  static react::uwp::ImageSource parseJson(folly::dynamic& json)
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
struct json_type_traits<winrt::Windows::UI::Xaml::Media::Stretch>
{
  static winrt::Windows::UI::Xaml::Media::Stretch parseJson(folly::dynamic& json)
  {
    winrt::Windows::UI::Xaml::Media::Stretch stretch;
    if (json == "cover")
    {
      stretch = winrt::Windows::UI::Xaml::Media::Stretch::UniformToFill;
    }
    else if (json == "contain")
    {
      stretch = winrt::Windows::UI::Xaml::Media::Stretch::Uniform;
    }
    else
    {
      stretch = winrt::Windows::UI::Xaml::Media::Stretch::Fill;
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
    winrt::Image image;
    image.Stretch(winrt::Stretch::UniformToFill);
    return image;
  }

  void ImageViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
  {
    auto image = nodeToUpdate->GetView().as<winrt::Image>();
    if (image == nullptr)
      return;

    for (auto& pair : reactDiffMap.items())
    {
      if (pair.first == "source")
      {
        setSource(image, pair.second);
      }
      else if (pair.first == "resizeMode")
      {
        auto stretch = json_type_traits<winrt::Windows::UI::Xaml::Media::Stretch>::parseJson(pair.second);
        image.Stretch(stretch);
      }

      // TODO: overflow
    }

    Super::UpdateProperties(nodeToUpdate, reactDiffMap);
  }

  void EmitImageEvent(const std::shared_ptr<react::uwp::IReactInstance> &reactInstance, winrt::Image image, const char* eventName, ImageSource& source)
  {
    if (reactInstance == nullptr)
      return;

    int64_t tag = image.Tag().as<winrt::IPropertyValue>().GetInt64();
    folly::dynamic imageSource = folly::dynamic::object()
      ("url", source.uri)
      ("width", source.width)
      ("height", source.height);

    folly::dynamic eventData = folly::dynamic::object()
      ("target", tag)
      ("source", imageSource);
    reactInstance->DispatchEvent(tag, eventName, std::move(eventData));
  }

  std::future<void> DownloadImageAsync(winrt::Image image, ImageSource source, std::weak_ptr<IReactInstance> instanceWeak)
  {
    // Since this is a fire and forget function (not waited upon when called), we need to wrap in a
    // try/catch or uncaught exceptions will take down the process
    try
    {
      winrt::Windows::Web::Http::HttpClient httpClient;
      winrt::Windows::Web::Http::HttpMethod httpMethod(nullptr);
      if (source.method.empty())
        httpMethod = winrt::Windows::Web::Http::HttpMethod::Get();
      else
        httpMethod = winrt::Windows::Web::Http::HttpMethod(facebook::react::unicode::Utf8ToUtf16(source.method));
      winrt::Windows::Foundation::Uri uri(facebook::react::unicode::Utf8ToUtf16(source.uri));

      winrt::Windows::Web::Http::HttpRequestMessage request(httpMethod, uri);

      if (!source.headers.empty())
      {
        for (auto& header : source.headers.items())
        {
          auto& name = header.first.getString();
          auto& value = header.second.getString();

          request.Headers().Append(facebook::react::unicode::Utf8ToUtf16(name), facebook::react::unicode::Utf8ToUtf16(value));
        }
      }

      winrt::Windows::Web::Http::HttpResponseMessage response = co_await httpClient.SendRequestAsync(request);

      auto instance = instanceWeak.lock();
      if (response.StatusCode() == winrt::Windows::Web::Http::HttpStatusCode::Ok)
      {
        winrt::Windows::Storage::Streams::IInputStream inputStream = co_await response.Content().ReadAsInputStreamAsync();
        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream memoryStream;
        co_await winrt::Windows::Storage::Streams::RandomAccessStream::CopyAsync(inputStream, memoryStream);
        memoryStream.Seek(0);

        winrt::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(memoryStream);
        image.Source(bitmap);

        EmitImageEvent(instance, image, "topLoad", source);
      }
      else
      {
        EmitImageEvent(instance, image, "topError", source);
      }
    }
    catch (winrt::hresult_error const &)
    {
      EmitImageEvent(instanceWeak.lock(), image, "topError", source);
    }

    EmitImageEvent(instanceWeak.lock(), image, "topLoadEnd", source);
  }

  void ImageViewManager::setSource(winrt::Image image, folly::dynamic& data)
  {
    auto instance = m_wkReactInstance.lock();
    if (instance == nullptr)
      return;

    auto sources = json_type_traits<std::vector<ImageSource>>::parseJson(data);
    auto uriString = sources[0].uri;
    if (uriString.length() == 0)
    {
      EmitImageEvent(instance, image, "topError", sources[0]);
      return;
    }

    if (sources[0].packagerAsset && uriString.find("assets") == 0)
      uriString.replace(0, 6, "ms-appx://");

    bool needsDownload = false;

    try
    {
      auto uri = winrt::Uri(winrt::hstring(asWStr(uriString).c_str()));
      auto scheme = uri.SchemeName();
      needsDownload = scheme == L"http" || scheme == L"https";

      EmitImageEvent(instance, image, "topLoadStart", sources[0]);
      if (needsDownload)
      {
        // FUTURE: This should get a weak_ptr from instance
        // fix when the win32 instance merge happens
        DownloadImageAsync(image, sources[0], instance);
      }
      else
      {
        winrt::BitmapImage bitmap;
        bitmap.UriSource(uri);
        image.Source(bitmap);

        EmitImageEvent(instance, image, "topLoad", sources[0]);
      }
    }
    catch (...)
    {
      OutputDebugString(L"caught exception setting up image source");
      EmitImageEvent(instance, image, "topError", sources[0]);
    }

    if (!needsDownload)
      EmitImageEvent(instance, image, "topLoadEnd", sources[0]);
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

private:
  ImageViewManagerModule *m_parent;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
};

winrt::fire_and_forget GetImageSizeAsync(std::string uri, facebook::xplat::module::CxxModule::Callback successCallback, facebook::xplat::module::CxxModule::Callback errorCallback)
{
  winrt::Image image;
  ImageSource source;
  source.uri = uri;

  winrt::apartment_context ui_thread;

  bool succeeded = false;
  try
  {
    co_await DownloadImageAsync(image, source, std::weak_ptr<IReactInstance>());

    // Get back to the UI-thread to read properties on Image
    co_await ui_thread;
    auto bitmap = image.Source().try_as<winrt::BitmapImage>();
    if (bitmap)
    {
      folly::dynamic sizes = folly::dynamic::object("width", bitmap.PixelWidth())("height", bitmap.PixelHeight());
      successCallback({ sizes });
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
    }),
    Method("prefetchImage", [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback)
    {
      imageViewManager->prefetchImage(facebook::xplat::jsArgAsString(args, 0), successCallback, errorCallback);
    }),
  };
}


} }
