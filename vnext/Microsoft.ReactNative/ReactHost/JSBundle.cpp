// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "JSBundle.h"
#include <vector>

namespace Mso::React {

static const JSBundleRegistration *g_jsBundleRegistration{nullptr};

LIBLET_PUBLICAPI JSBundleRegistration::JSBundleRegistration(
    const char *jsBundleId,
    const char *jsBundleFileName) noexcept
    : m_jsBundleId{jsBundleId}, m_jsBundleFileName{jsBundleFileName}, m_next{g_jsBundleRegistration} {
  g_jsBundleRegistration = this;
}

LIBLET_PUBLICAPI const JSBundleRegistration &GetBundleRegistration(std::string_view jsBundleId) noexcept {
  for (auto current = g_jsBundleRegistration; current != nullptr; current = current->Next()) {
    if (strcmp(jsBundleId.data(), current->JSBundleId()) == 0) {
      return *current;
    }
  }

  VerifyElseCrashSzTag(false, "JS bundle registration not found", 0x0248a05f /* tag_cskb5 */);
}

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeRegisteredJSBundle(std::string_view jsBundleId) noexcept {
  return GetBundleRegistration(jsBundleId).MakeJSBundle();
}

LIBLET_PUBLICAPI std::vector<Mso::CntPtr<IJSBundle>> GetRegisteredJSBundles(
    std::initializer_list<std::string_view> jsBundleIds) noexcept {
  std::vector<Mso::CntPtr<IJSBundle>> jsBundles;

  AddRegisteredJSBundles(jsBundles, jsBundleIds);

  return jsBundles;
}

LIBLET_PUBLICAPI void AddRegisteredJSBundles(
    std::vector<Mso::CntPtr<IJSBundle>> &jsBundles,
    std::initializer_list<std::string_view> jsBundleIds) noexcept {
  for (const auto &jsBundleId : jsBundleIds) {
    jsBundles.push_back(MakeRegisteredJSBundle(jsBundleId));
  }
}

// Append only ASCII characters and escape the most common control characters
void AppendEscapedASCIIJSString(std::string &target, const std::string &jsonValue) {
  int escapedSize{2}; // For "" start and end
  for (auto ch : jsonValue) {
    if (ch < 127) // ASCII characters
    {
      if (ch >= ' ') // non-control character - escape \ and "
      {
        ++escapedSize;
        if (ch == '\\')
          ++escapedSize;
        else if (ch == '"')
          ++escapedSize;
      } else // a control character - escape most common ones and skip others
      {
        if (ch == '\r')
          escapedSize += 2;
        else if (ch == '\n')
          escapedSize += 2;
        else if (ch == '\t')
          escapedSize += 2;
        else if (ch == '\b')
          escapedSize += 2;
        else if (ch == '\v')
          escapedSize += 2;
        else if (ch == '\f')
          escapedSize += 2;
      }
    }
  }

  target.append(escapedSize, '\0');
  char *ptr = &target[target.length() - escapedSize];
  *ptr++ = '"'; // string start
  for (auto ch : jsonValue) {
    if (ch < 127) // ASCII characters
    {
      if (ch >= ' ') // non-control character - escape \ and "
      {
        if (ch == '\\')
          *ptr++ = '\\', *ptr++ = '\\';
        else if (ch == '"')
          *ptr++ = '\\', *ptr++ = '"';
        else
          *ptr++ = ch;
      } else // a control character - escape most common ones and skip others
      {
        if (ch == '\r')
          *ptr++ = '\\', *ptr++ = 'r';
        else if (ch == '\n')
          *ptr++ = '\\', *ptr++ = 'n';
        else if (ch == '\t')
          *ptr++ = '\\', *ptr++ = 't';
        else if (ch == '\b')
          *ptr++ = '\\', *ptr++ = 'b';
        else if (ch == '\v')
          *ptr++ = '\\', *ptr++ = 'v';
        else if (ch == '\f')
          *ptr++ = '\\', *ptr++ = 'f';
      }
    }
  }
  *ptr++ = '"'; // string end
}

struct DynamicJSBundle : Mso::UnknownObject<IJSBundle> {
  DynamicJSBundle(std::string &&id, std::string &&value) noexcept : m_id{std::move(id)}, m_value{std::move(value)} {}

  std::string_view Content() noexcept override {
    return {m_value.data(), m_value.size()};
  }

  JSBundleInfo Info() noexcept override {
    return {std::string{m_id}, "", 0};
  }

 private:
  const std::string m_id;
  const std::string m_value;
};

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeGlobalVariableJSBundle(
    const std::string &variableName,
    const std::string &jsonValue) noexcept {
  std::string jsText = variableName + " = JSON.parse(";
  AppendEscapedASCIIJSString(jsText, jsonValue);
  jsText += ");";
  return MakeDynamicJSBundle("GlobalVar:" + variableName, std::move(jsText));
}

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeDynamicJSBundle(
    std::string &&jsBundleId,
    std::string &&jsBundleText) noexcept {
  return Mso::Make<DynamicJSBundle, IJSBundle>(std::move(jsBundleId), std::move(jsBundleText));
}

} // namespace Mso::React
