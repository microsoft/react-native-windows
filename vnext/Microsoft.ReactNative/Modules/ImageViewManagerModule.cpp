// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// NYI:
//   implement image cache
//   implement prefetch functionality
//   implement multi source (parse out most suitable image source from array of
//   sources)
#include "pch.h"

#include "ImageViewManagerModule.h"

#include <UI.Xaml.Media.Imaging.h>
#include <Views/Image/ReactImage.h>
#include <cxxreact/JsArgumentHelpers.h>
#include "Unicode.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace xaml::Media::Imaging;
} // namespace winrt

namespace react::uwp {
//
// ImageViewManagerModule::ImageViewManagerModuleImpl
//
class ImageViewManagerModule::ImageViewManagerModuleImpl {
 public:
  ImageViewManagerModuleImpl(ImageViewManagerModule *parent) : m_parent(parent) {}

  void Disconnect() {
    m_parent = nullptr;
  }

  void getSize(std::string uri, Callback successCallback, Callback errorCallback);
  void getSizeWithHeaders(std::string uri, folly::dynamic &headers, Callback successCallback, Callback errorCallback);
  void prefetchImage(std::string uri, Callback successCallback, Callback errorCallback);
  void queryCache(const folly::dynamic &requests, Callback successCallback, Callback errorCallback);

 private:
  ImageViewManagerModule *m_parent;
};

winrt::fire_and_forget GetImageSizeAsync(
    std::string uriString,
    folly::dynamic &headers,
    facebook::xplat::module::CxxModule::Callback successCallback,
    facebook::xplat::module::CxxModule::Callback errorCallback) {
  bool succeeded{false};

  try {
    ReactImageSource source;
    source.uri = uriString;
    source.headers = headers;

    winrt::Uri uri{Microsoft::Common::Unicode::Utf8ToUtf16(uriString)};
    winrt::hstring scheme{uri.SchemeName()};
    bool needsDownload = (scheme == L"http") || (scheme == L"https");
    bool inlineData = scheme == L"data";

    winrt::InMemoryRandomAccessStream memoryStream;
    if (needsDownload) {
      memoryStream = co_await GetImageStreamAsync(source);
    } else if (inlineData) {
      memoryStream = co_await GetImageInlineDataAsync(source);
    }

    winrt::BitmapImage bitmap;
    if (memoryStream) {
      co_await bitmap.SetSourceAsync(memoryStream);
    }

    if (bitmap) {
      successCallback({folly::dynamic::array(bitmap.PixelWidth(), bitmap.PixelHeight())});
      succeeded = true;
    }
  } catch (winrt::hresult_error const &) {
  }

  if (!succeeded)
    errorCallback({});

  co_return;
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::getSize(
    std::string uri,
    Callback successCallback,
    Callback errorCallback) {
  folly::dynamic headers{};
  GetImageSizeAsync(uri, headers, successCallback, errorCallback);
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::getSizeWithHeaders(
    std::string uri,
    folly::dynamic &headers,
    Callback successCallback,
    Callback errorCallback) {
  GetImageSizeAsync(uri, headers, successCallback, errorCallback);
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::prefetchImage(
    std::string /*uri*/,
    Callback successCallback,
    Callback /*errorCallback*/) {
  // NotYetImplemented
  successCallback({});
}

void ImageViewManagerModule::ImageViewManagerModuleImpl::queryCache(
    const folly::dynamic & /*requests*/,
    Callback successCallback,
    Callback /*errorCallback*/) {
  // NotYetImplemented
  successCallback({folly::dynamic::object()});
}

//
// ImageViewManagerModule
//
const char *ImageViewManagerModule::name = "ImageLoader";

ImageViewManagerModule::ImageViewManagerModule()
    : m_imageViewManagerModule(std::make_shared<ImageViewManagerModuleImpl>(this)) {}

ImageViewManagerModule::~ImageViewManagerModule() {}

std::string ImageViewManagerModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> ImageViewManagerModule::getConstants() {
  return {{}};
}

auto ImageViewManagerModule::getMethods() -> std::vector<Method> {
  std::shared_ptr<ImageViewManagerModuleImpl> imageViewManager(m_imageViewManagerModule);
  return {
      Method(
          "getSize",
          [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback) {
            imageViewManager->getSize(facebook::xplat::jsArgAsString(args, 0), successCallback, errorCallback);
          }),
      Method(
          "getSizeWithHeaders",
          [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback) {
            imageViewManager->getSizeWithHeaders(
                facebook::xplat::jsArgAsString(args, 0),
                facebook::xplat::jsArgAsObject(args, 1),
                successCallback,
                errorCallback);
          }),
      Method(
          "prefetchImage",
          [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback) {
            imageViewManager->prefetchImage(facebook::xplat::jsArgAsString(args, 0), successCallback, errorCallback);
          }),
      Method(
          "queryCache",
          [imageViewManager](folly::dynamic args, Callback successCallback, Callback errorCallback) {
            imageViewManager->queryCache(args[0], successCallback, errorCallback);
          }),
  };
}

} // namespace react::uwp
