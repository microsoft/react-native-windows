// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include "functional/functor.h"
#include "functional/functorRef.h"

namespace Mso::React {

struct JSBundleInfo {
  std::string Id;
  std::string FileName;
  uint64_t Timestamp;
};

MSO_STRUCT_GUID(IJSBundle, "9097ad91-7bbb-41b2-9575-f9d262b4be1d")
struct IJSBundle : IUnknown {
  virtual std::string_view Content() noexcept = 0;
  virtual JSBundleInfo Info() noexcept = 0;
};

// A base class to implement static bundle registration.
// Note that it does not manage lifetime of bundleId and bundleFileName because it
// expects them to be static strings.
struct JSBundleRegistration {
  LIBLET_PUBLICAPI JSBundleRegistration(const char *jsBundleId, const char *jsBundleFileName) noexcept;

  const char *JSBundleId() const noexcept {
    return m_jsBundleId;
  }
  const char *JSBundleFileName() const noexcept {
    return m_jsBundleFileName;
  }
  const JSBundleRegistration *Next() const noexcept {
    return m_next;
  }

  virtual Mso::CntPtr<IJSBundle> MakeJSBundle() const noexcept = 0;

 private:
  const char *m_jsBundleId{nullptr};
  const char *m_jsBundleFileName{nullptr};
  const JSBundleRegistration *m_next{nullptr};
};

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeRegisteredJSBundle(std::string_view jsBundleId) noexcept;

LIBLET_PUBLICAPI std::vector<Mso::CntPtr<IJSBundle>> GetRegisteredJSBundles(
    std::initializer_list<std::string_view> jsBundleIds) noexcept;

LIBLET_PUBLICAPI void AddRegisteredJSBundles(
    std::vector<Mso::CntPtr<IJSBundle>> &jsBundles,
    std::initializer_list<std::string_view> jsBundleIds) noexcept;

LIBLET_PUBLICAPI const JSBundleRegistration &GetBundleRegistration(std::string_view jsBundleId) noexcept;

// Implements static file bundle registration.
// Note that it does not manage lifetime of bundleId and bundleFileName because it
// expects them to be static strings.
struct JSFileBundleRegistration : public JSBundleRegistration {
  LIBLET_PUBLICAPI JSFileBundleRegistration(const char *jsBundleId, const char *jsBundleFileName) noexcept;

  Mso::CntPtr<IJSBundle> MakeJSBundle() const noexcept override;
};

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeFileJSBundle(std::string &&id, std::string &&fileName) noexcept;

LIBLET_PUBLICAPI std::string GetJSBundleFilePath(
    const std::string &jsBundleBasePath,
    const std::string &jsBundleRelativePath) noexcept;

//! Creates a dynamic JS Bundle based on a string.
//! We do not generate byte code for this bundle.
//! It can be useful in unit test scenarios or for short dynamically
//! generated scripts.
LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeDynamicJSBundle(
    std::string &&jsBundleId,
    std::string &&jsBundleText) noexcept;

//! Creates a dynamic JS Bundle that sets a global variable to
//! a JSON value which is an object or array.
//! We do not generate byte code for this bundle.
LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeGlobalVariableJSBundle(
    const std::string &variableName,
    const std::string &jsonValue) noexcept;

} // namespace Mso::React
