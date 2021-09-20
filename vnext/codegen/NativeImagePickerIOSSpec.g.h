
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

REACT_STRUCT(ImagePickerIOSSpec_openCameraDialog_config)
struct ImagePickerIOSSpec_openCameraDialog_config {
    REACT_FIELD(unmirrorFrontFacingCamera)
    bool unmirrorFrontFacingCamera;
    REACT_FIELD(videoMode)
    bool videoMode;
};

REACT_STRUCT(ImagePickerIOSSpec_openSelectDialog_config)
struct ImagePickerIOSSpec_openSelectDialog_config {
    REACT_FIELD(showImages)
    bool showImages;
    REACT_FIELD(showVideos)
    bool showVideos;
};

struct ImagePickerIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Callback<bool>) noexcept>{0, L"canRecordVideos"},
      Method<void(Callback<bool>) noexcept>{1, L"canUseCamera"},
      Method<void(ImagePickerIOSSpec_openCameraDialog_config, Callback<std::string, double, double>, Callback<>) noexcept>{2, L"openCameraDialog"},
      Method<void(ImagePickerIOSSpec_openSelectDialog_config, Callback<std::string, double, double>, Callback<>) noexcept>{3, L"openSelectDialog"},
      Method<void() noexcept>{4, L"clearAllPendingVideos"},
      Method<void(std::string) noexcept>{5, L"removePendingVideo"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ImagePickerIOSSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "canRecordVideos",
          "    REACT_METHOD(canRecordVideos) void canRecordVideos(std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(canRecordVideos) static void canRecordVideos(std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "canUseCamera",
          "    REACT_METHOD(canUseCamera) void canUseCamera(std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(canUseCamera) static void canUseCamera(std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "openCameraDialog",
          "    REACT_METHOD(openCameraDialog) void openCameraDialog(ImagePickerIOSSpec_openCameraDialog_config && config, std::function<void(std::string, double, double)> const & successCallback, std::function<void()> const & cancelCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(openCameraDialog) static void openCameraDialog(ImagePickerIOSSpec_openCameraDialog_config && config, std::function<void(std::string, double, double)> const & successCallback, std::function<void()> const & cancelCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "openSelectDialog",
          "    REACT_METHOD(openSelectDialog) void openSelectDialog(ImagePickerIOSSpec_openSelectDialog_config && config, std::function<void(std::string, double, double)> const & successCallback, std::function<void()> const & cancelCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(openSelectDialog) static void openSelectDialog(ImagePickerIOSSpec_openSelectDialog_config && config, std::function<void(std::string, double, double)> const & successCallback, std::function<void()> const & cancelCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "clearAllPendingVideos",
          "    REACT_METHOD(clearAllPendingVideos) void clearAllPendingVideos() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(clearAllPendingVideos) static void clearAllPendingVideos() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "removePendingVideo",
          "    REACT_METHOD(removePendingVideo) void removePendingVideo(std::string url) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removePendingVideo) static void removePendingVideo(std::string url) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
