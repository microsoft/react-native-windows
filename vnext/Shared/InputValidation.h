// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace Microsoft::ReactNative::InputValidation {

// Security exceptions for validation failures
class ValidationException : public std::runtime_error {
 public:
  explicit ValidationException(const std::string &message) : std::runtime_error(message) {}
};

// Specific validation exception types
class InvalidSizeException : public std::logic_error {
 public:
  explicit InvalidSizeException(const std::string &message) : std::logic_error(message) {}
};

class InvalidEncodingException : public std::logic_error {
 public:
  explicit InvalidEncodingException(const std::string &message) : std::logic_error(message) {}
};

class InvalidPathException : public std::logic_error {
 public:
  explicit InvalidPathException(const std::string &message) : std::logic_error(message) {}
};

class InvalidURLException : public std::logic_error {
 public:
  explicit InvalidURLException(const std::string &message) : std::logic_error(message) {}
};

// Centralized allowlists for encodings
namespace AllowedEncodings {
static const std::vector<std::string> FILE_READER_ENCODINGS = {
    "UTF-8",
    "utf-8",
    "utf8",
    "UTF-16",
    "utf-16",
    "utf16",
    "ASCII",
    "ascii",
    "ISO-8859-1",
    "iso-8859-1",
    "" // Empty is allowed (defaults to UTF-8)
};
} // namespace AllowedEncodings

// Centralized URL scheme allowlists
namespace AllowedSchemes {
static const std::vector<std::string> HTTP_SCHEMES = {"http", "https"};
static const std::vector<std::string> WEBSOCKET_SCHEMES = {"ws", "wss"};
static const std::vector<std::string> FILE_SCHEMES = {"file"};
static const std::vector<std::string> LINKING_SCHEMES = {"http", "https", "mailto", "tel", "ms-settings"};
static const std::vector<std::string> IMAGE_SCHEMES = {"http", "https"};
static const std::vector<std::string> DEBUG_SCHEMES = {"http", "https", "file"};
} // namespace AllowedSchemes

// Logging callback for validation failures (SDL requirement)
using ValidationLogger = std::function<void(const std::string &category, const std::string &message)>;
void SetValidationLogger(ValidationLogger logger);
void LogValidationFailure(const std::string &category, const std::string &message);

// URL/URI Validation - Protects against SSRF (100% SDL Compliant)
class URLValidator {
 public:
  // Validate URL with scheme allowlist (SDL compliant)
  // Includes: URL decoding loop, DNS resolution, private IP blocking
  // allowLocalhost: Set to true for testing/development scenarios only
  static void ValidateURL(
      const std::string &url,
      const std::vector<std::string> &allowedSchemes = {"http", "https"},
      bool allowLocalhost = false);

  // Validate URL with DNS resolution (async version for production)
  // Resolves hostname and checks if resolved IP is private
  static void ValidateURLWithDNS(
      const std::string &url,
      const std::vector<std::string> &allowedSchemes = {"http", "https"},
      bool allowLocalhost = false);

  // Check if hostname is private IP/localhost (expanded for SDL)
  static bool IsPrivateOrLocalhost(const std::string &hostname);

  // URL decode with loop until no further decoding (SDL requirement)
  static std::string DecodeURL(const std::string &url);

  // Extract hostname from URL
  static std::string ExtractHostname(const std::string &url);

  // Check if IP is in private range (supports IPv4/IPv6)
  static bool IsPrivateIP(const std::string &ip);

  // Resolve hostname to IP addresses (for DNS rebinding protection)
  static std::vector<std::string> ResolveHostname(const std::string &hostname);

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

  // Production limits (strict SDL compliance)
  static constexpr size_t STRICT_MAX_BLOB_SIZE = 50 * 1024 * 1024; // 50MB
  static constexpr size_t STRICT_MAX_WEBSOCKET_FRAME = 64 * 1024 * 1024; // 64MB
  static constexpr size_t STRICT_MAX_DATA_URI_SIZE = 5 * 1024 * 1024; // 5MB
  static constexpr size_t STRICT_MAX_HEADER_LENGTH = 4096; // 4KB

  // Developer-friendly limits (platform default)
  static constexpr size_t DEV_MAX_BLOB_SIZE = 500 * 1024 * 1024; // 500MB
  static constexpr size_t DEV_MAX_WEBSOCKET_FRAME = 1024 * 1024 * 1024; // 1GB
  static constexpr size_t DEV_MAX_DATA_URI_SIZE = 100 * 1024 * 1024; // 100MB
  static constexpr size_t DEV_MAX_HEADER_LENGTH = 32768; // 32KB

  // Fixed constants (not configurable)
  static constexpr size_t MAX_CLOSE_REASON = 123; // WebSocket spec
  static constexpr size_t MAX_URL_LENGTH = 2048; // URL max

  // Legacy constants (deprecated - use GetMaxBlobSize() etc.)
  static constexpr size_t MAX_BLOB_SIZE = DEV_MAX_BLOB_SIZE;
  static constexpr size_t MAX_WEBSOCKET_FRAME = DEV_MAX_WEBSOCKET_FRAME;
  static constexpr size_t MAX_HEADER_LENGTH = DEV_MAX_HEADER_LENGTH;
  static constexpr size_t MAX_DATA_URI_SIZE = DEV_MAX_DATA_URI_SIZE;

  // Smart getters that respect RNW_STRICT_SDL flag
  static size_t GetMaxBlobSize();
  static size_t GetMaxWebSocketFrame();
  static size_t GetMaxDataUriSize();
  static size_t GetMaxHeaderLength();
};

// Encoding Validation - Protects against malformed data (SDL compliant)
class EncodingValidator {
 public:
  // Validate base64 string format
  static bool IsValidBase64(const std::string &str);

  // Estimate decoded size of base64 string
  static size_t EstimateBase64DecodedSize(std::string_view base64String);

  // Check for CRLF injection in headers (SDL requirement)
  static bool ContainsCRLF(std::string_view str);

  // Validate header value (no CRLF, length limit)
  static void ValidateHeaderValue(std::string_view value);

 private:
  static const std::regex BASE64_REGEX;
};

} // namespace Microsoft::ReactNative::InputValidation
