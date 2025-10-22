// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "../Shared/InputValidation.h"

using namespace Microsoft::ReactNative::InputValidation;

// ============================================================================
// SDL COMPLIANCE TESTS - URL Validation (SSRF Prevention)
// ============================================================================

TEST(URLValidatorTest, AllowsHTTPSchemesOnly) {
  // Positive: http and https allowed
  EXPECT_NO_THROW(URLValidator::ValidateURL("http://example.com", {"http", "https"}));
  EXPECT_NO_THROW(URLValidator::ValidateURL("https://example.com", {"http", "https"}));
  
  // Negative: file, ftp, javascript blocked
  EXPECT_THROW(URLValidator::ValidateURL("file:///etc/passwd", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("ftp://example.com", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("javascript:alert(1)", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksLocalhostVariants) {
  // SDL Test Case: Block localhost
  EXPECT_THROW(URLValidator::ValidateURL("https://localhost/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://localHoSt/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://ip6-localhost/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksLoopbackIPs) {
  // SDL Test Case: Block 127.x.x.x
  EXPECT_THROW(URLValidator::ValidateURL("https://127.0.0.1/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://127.0.1.2/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://127.255.255.255/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksIPv6Loopback) {
  // SDL Test Case: Block ::1
  EXPECT_THROW(URLValidator::ValidateURL("https://[::1]/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://[0:0:0:0:0:0:0:1]/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksAWSMetadata) {
  // SDL Test Case: Block 169.254.169.254
  EXPECT_THROW(URLValidator::ValidateURL("http://169.254.169.254/latest/meta-data/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksPrivateIPRanges) {
  // SDL Test Case: Block private IPs
  EXPECT_THROW(URLValidator::ValidateURL("https://10.0.0.1/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://192.168.1.1/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://172.16.0.1/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://172.31.255.255/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksIPv6PrivateRanges) {
  // SDL Test Case: Block fc00::/7 and fe80::/10
  EXPECT_THROW(URLValidator::ValidateURL("https://[fc00::]/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://[fe80::]/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://[fd00::]/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, DecodesDoubleEncodedURLs) {
  // SDL Requirement: Decode URLs until no further decoding possible
  // %252e%252e = %2e%2e = .. (double encoded)
  std::string url = "https://example.com/%252e%252e/etc/passwd";
  std::string decoded = URLValidator::DecodeURL(url);
  EXPECT_TRUE(decoded.find("..") != std::string::npos);
}

TEST(URLValidatorTest, EnforcesMaxLength) {
  // SDL: URL length limit (2048 bytes)
  std::string longURL = "https://example.com/" + std::string(3000, 'a');
  EXPECT_THROW(URLValidator::ValidateURL(longURL, {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, AllowsPublicURLs) {
  // Positive: Public URLs should work
  EXPECT_NO_THROW(URLValidator::ValidateURL("https://example.com/api/data", {"http", "https"}));
  EXPECT_NO_THROW(URLValidator::ValidateURL("https://github.com/microsoft/react-native-windows", {"http", "https"}));
}

// ============================================================================
// SDL COMPLIANCE TESTS - Path Traversal Prevention
// ============================================================================

TEST(PathValidatorTest, DetectsBasicTraversal) {
  // SDL Test Case: Detect ../
  EXPECT_TRUE(PathValidator::ContainsTraversal("../../etc/passwd"));
  EXPECT_TRUE(PathValidator::ContainsTraversal("..\\..\\windows\\system32"));
  EXPECT_TRUE(PathValidator::ContainsTraversal("/../../OtherPath/"));
}

TEST(PathValidatorTest, DetectsEncodedTraversal) {
  // SDL Test Case: Detect %2e%2e
  EXPECT_TRUE(PathValidator::ContainsTraversal("%2e%2e%2f%2e%2e%2fOtherPath"));
  EXPECT_TRUE(PathValidator::ContainsTraversal("/%2E%2E/etc/passwd"));
}

TEST(PathValidatorTest, DetectsDoubleEncodedTraversal) {
  // SDL Test Case: Detect %252e%252e (double encoded)
  EXPECT_TRUE(PathValidator::ContainsTraversal("%252e%252e%252f"));
  EXPECT_TRUE(PathValidator::ContainsTraversal("/%252E%252E%252fOtherPath/"));
}

TEST(PathValidatorTest, DetectsEncodedBackslash) {
  // SDL Test Case: Detect %5c (backslash)
  EXPECT_TRUE(PathValidator::ContainsTraversal("%5c%5c"));
  EXPECT_TRUE(PathValidator::ContainsTraversal("%255c%255c")); // Double encoded
}

TEST(PathValidatorTest, ValidBlobIDFormat) {
  // Positive: Valid blob IDs
  EXPECT_NO_THROW(PathValidator::ValidateBlobId("blob123"));
  EXPECT_NO_THROW(PathValidator::ValidateBlobId("abc-def_123"));
  EXPECT_NO_THROW(PathValidator::ValidateBlobId("A1B2C3"));
}

TEST(PathValidatorTest, InvalidBlobIDFormats) {
  // Negative: Invalid characters
  EXPECT_THROW(PathValidator::ValidateBlobId("blob/../etc"), ValidationException);
  EXPECT_THROW(PathValidator::ValidateBlobId("blob/file"), ValidationException);
  EXPECT_THROW(PathValidator::ValidateBlobId("blob\\file"), ValidationException);
}

TEST(PathValidatorTest, BlobIDLengthLimit) {
  // SDL: Max 128 characters
  std::string validLength(128, 'a');
  EXPECT_NO_THROW(PathValidator::ValidateBlobId(validLength));
  
  std::string tooLong(129, 'a');
  EXPECT_THROW(PathValidator::ValidateBlobId(tooLong), ValidationException);
}

TEST(PathValidatorTest, BundlePathTraversalBlocked) {
  // SDL: Block path traversal in bundle paths
  EXPECT_THROW(PathValidator::ValidateFilePath("../../etc/passwd", "C:\\app"), ValidationException);
  EXPECT_THROW(PathValidator::ValidateFilePath("..\\..\\windows", "C:\\app"), ValidationException);
  EXPECT_THROW(PathValidator::ValidateFilePath("%2e%2e%2f", "C:\\app"), ValidationException);
}

// ============================================================================
// SDL COMPLIANCE TESTS - Size Validation (DoS Prevention)
// ============================================================================

TEST(SizeValidatorTest, EnforcesMaxBlobSize) {
  // SDL: 100MB max
  EXPECT_NO_THROW(SizeValidator::ValidateSize(100 * 1024 * 1024, SizeValidator::MAX_BLOB_SIZE, "Blob"));
  EXPECT_THROW(SizeValidator::ValidateSize(101 * 1024 * 1024, SizeValidator::MAX_BLOB_SIZE, "Blob"), ValidationException);
}

TEST(SizeValidatorTest, EnforcesMaxWebSocketFrame) {
  // SDL: 256MB max
  EXPECT_NO_THROW(SizeValidator::ValidateSize(256 * 1024 * 1024, SizeValidator::MAX_WEBSOCKET_FRAME, "WebSocket"));
  EXPECT_THROW(SizeValidator::ValidateSize(257 * 1024 * 1024, SizeValidator::MAX_WEBSOCKET_FRAME, "WebSocket"), ValidationException);
}

TEST(SizeValidatorTest, EnforcesCloseReasonLimit) {
  // SDL: 123 bytes max (WebSocket spec)
  EXPECT_NO_THROW(SizeValidator::ValidateSize(123, SizeValidator::MAX_CLOSE_REASON, "Close reason"));
  EXPECT_THROW(SizeValidator::ValidateSize(124, SizeValidator::MAX_CLOSE_REASON, "Close reason"), ValidationException);
}

// ============================================================================
// SDL COMPLIANCE TESTS - Encoding Validation
// ============================================================================

TEST(EncodingValidatorTest, ValidBase64Format) {
  // Positive: Valid base64
  EXPECT_TRUE(EncodingValidator::IsValidBase64("SGVsbG8gV29ybGQ="));
  EXPECT_TRUE(EncodingValidator::IsValidBase64("YWJjZGVmZ2hpamtsbW5vcA=="));
}

TEST(EncodingValidatorTest, InvalidBase64Format) {
  // Negative: Invalid base64
  EXPECT_FALSE(EncodingValidator::IsValidBase64("Not@Valid!"));
  EXPECT_FALSE(EncodingValidator::IsValidBase64("")); // Empty
}

// ============================================================================
// SDL COMPLIANCE TESTS - Numeric Validation
// ============================================================================


// ============================================================================
// SDL COMPLIANCE TESTS - Header CRLF Injection Prevention
// ============================================================================


// ============================================================================
// SDL COMPLIANCE TESTS - Logging
// ============================================================================

TEST(ValidationLoggerTest, LogsFailures) {
  // Trigger validation failure to test logging
  try {
    URLValidator::ValidateURL("https://localhost/", {"http", "https"});
    FAIL() << "Expected ValidationException";
  } catch (const ValidationException& ex) {
    // Verify exception message is meaningful
    std::string message = ex.what();
    EXPECT_FALSE(message.empty());
    EXPECT_TRUE(message.find("localhost") != std::string::npos || message.find("SSRF") != std::string::npos);
  }
}

