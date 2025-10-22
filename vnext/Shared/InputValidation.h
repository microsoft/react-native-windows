// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace Microsoft::ReactNative::InputValidation {

// Security exception for validation failures
class ValidationException : public std::runtime_error {
 public:
  explicit ValidationException(const std::string &message) : std::runtime_error(message) {}
};

// Logging callback for validation failures (SDL requirement)
using ValidationLogger = std::function<void(const std::string &category, const std::string &message)>;
void SetValidationLogger(ValidationLogger logger);
void LogValidationFailure(const std::string &category, const std::string &message);

// URL/URI Validation - Protects against SSRF (100% SDL Compliant)
class URLValidator {
 public:
  // Validate URL with scheme allowlist (SDL compliant)
  // Includes: URL decoding loop, DNS resolution, private IP blocking
  static void ValidateURL(const std::string &url, const std::vector<std::string> &allowedSchemes = {"http", "https"});

  // Check if hostname is private IP/localhost (expanded for SDL)
  static bool IsPrivateOrLocalhost(const std::string &hostname);

  // URL decode with loop until no further decoding (SDL requirement)
  static std::string DecodeURL(const std::string &url);

  // Extract hostname from URL
  static std::string ExtractHostname(const std::string &url);

  // Check if IP is in private range (supports IPv4/IPv6)
  static bool IsPrivateIP(const std::string &ip);

 private:
  static const std::vector<std::string> BLOCKED_HOSTS;
  static bool IsOctalIPv4(const std::string &hostname);
  static bool IsHexIPv4(const std::string &hostname);
  static bool IsDecimalIPv4(const std::string &hostname);
};

// Path/BlobID Validation - Protects against path traversal (SDL compliant)
class PathValidator {
 public:
  // Check for directory traversal patterns (includes all encodings)
  static bool ContainsTraversal(const std::string &path);

  // Validate blob ID format (alphanumeric allowlist)
  static void ValidateBlobId(const std::string &blobId);

  // Validate file path for bundle loading (canonicalization)
  static void ValidateFilePath(const std::string &path, const std::string &baseDir);

  // Decode path and check for traversal (SDL decoding loop)
  static std::string DecodePath(const std::string &path);

 private:
  static const std::regex TRAVERSAL_REGEX;
  static const std::regex BLOB_ID_REGEX;
};

// Size Validation - Protects against DoS (SDL compliant)
class SizeValidator {
 public:
  // Validate size against maximum
  static void ValidateSize(size_t size, size_t maxSize, const char *context);

  // Validate numeric range (SDL requirement for signed/unsigned)
  static void ValidateInt32Range(int32_t value, int32_t min, int32_t max, const char *context);
  static void ValidateUInt32Range(uint32_t value, uint32_t min, uint32_t max, const char *context);

  // Constants for different types
  static constexpr size_t MAX_BLOB_SIZE = 100 * 1024 * 1024; // 100MB
  static constexpr size_t MAX_WEBSOCKET_FRAME = 256 * 1024 * 1024; // 256MB
  static constexpr size_t MAX_CLOSE_REASON = 123; // WebSocket spec
  static constexpr size_t MAX_URL_LENGTH = 2048; // URL max
  static constexpr size_t MAX_HEADER_LENGTH = 8192; // Header max
};

// Encoding Validation - Protects against malformed data (SDL compliant)
class EncodingValidator {
 public:
  // Validate base64 string format
  static bool IsValidBase64(const std::string &str);

  // Check for CRLF injection in headers (SDL requirement)
  static bool ContainsCRLF(const std::string &str);

  // Validate header value (no CRLF, length limit)
  static void ValidateHeaderValue(const std::string &value);

 private:
  static const std::regex BASE64_REGEX;
};

} // namespace Microsoft::ReactNative::InputValidation
