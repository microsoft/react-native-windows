// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "UriImageManager.h"

#include "Composition.ImageSource.g.h"
#include "Composition.UriImageManager.g.cpp"
#include <ReactPropertyBag.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ImageSource : ImageSourceT<ImageSource> {
  ImageSource(const facebook::react::ImageSource &source)
      : m_size({source.size.width, source.size.height}),
        m_scale(source.scale),
        m_uri(::Microsoft::ReactNative::UriTryCreate(winrt::to_hstring(source.uri))) {}

  winrt::Windows::Foundation::Uri Uri() noexcept {
    return m_uri;
  }

  winrt::Windows::Foundation::Size Size() noexcept {
    return m_size;
  }

  float Scale() noexcept {
    return m_scale;
  }

 private:
  const winrt::Windows::Foundation::Uri m_uri;
  const winrt::Windows::Foundation::Size m_size;
  const float m_scale;
};

winrt::Microsoft::ReactNative::Composition::ImageSource MakeImageSource(
    const facebook::react::ImageSource &source) noexcept {
  return winrt::make<ImageSource>(source);
}

static const ReactPropertyId<ReactNonAbiValue<winrt::com_ptr<UriImageManager>>> &UriImageManagerPropertyId() noexcept {
  static const ReactPropertyId<ReactNonAbiValue<winrt::com_ptr<UriImageManager>>> prop{
      L"ReactNative", L"UriImageManager"};
  return prop;
}

winrt::com_ptr<UriImageManager> UriImageManager::GetOrCreate(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept {
  auto uriImageManager =
      winrt::Microsoft::ReactNative::ReactPropertyBag(properties).GetOrCreate(UriImageManagerPropertyId(), []() {
        return winrt::make_self<UriImageManager>();
      });
  return uriImageManager.Value();
}

void UriImageManager::AddUriImageProvider(
    const winrt::Microsoft::ReactNative::IReactPropertyBag &properties,
    const IUriImageProvider &provider) {
  if (!provider)
    winrt::throw_hresult(E_INVALIDARG);
  GetOrCreate(winrt::Microsoft::ReactNative::ReactPropertyBag(properties))->m_providers.push_back(provider);
}

IUriImageProvider UriImageManager::TryGetUriImageProvider(
    const IReactContext &context,
    winrt::Microsoft::ReactNative::Composition::ImageSource &source) noexcept {
  auto uri = source.Uri();
  if (!uri) {
    return nullptr;
  }

  for (auto &provider : m_providers) {
    if (provider.CanLoadImageUri(context, source.Uri())) {
      return provider;
    }
  }

  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
