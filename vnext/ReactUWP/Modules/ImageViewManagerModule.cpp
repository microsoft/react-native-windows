// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// NYI:
//   implement image cache
//   implement prefetch functionality
//   implement multi source (parse out most suitable image source from array of sources)
#include "pch.h"

#include "ImageViewManagerModule.h"

#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

#include <cxxreact/JsArgumentHelpers.h>

#include <Views/Image/ReactImage.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace Windows::Storage::Streams;
  using namespace Windows::UI::Xaml::Media::Imaging;
}

namespace react { namespace uwp {
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
    ImageViewManagerModule* m_parent;
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

      winrt::BitmapImage bitmap;
      if (memoryStream)
      {
        co_await bitmap.SetSourceAsync(memoryStream);
      }

      if (bitmap)
      {
        successCallback({ bitmap.PixelWidth(), bitmap.PixelHeight() });
        succeeded = true;
      }
    }
    catch (winrt::hresult_error const&)
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

