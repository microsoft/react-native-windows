// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "InputValidation.h"
#include <gtest/gtest.h>

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
  EXPECT_THROW(
      URLValidator::ValidateURL("http://169.254.169.254/latest/meta-data/", {"http", "https"}), ValidationException);
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

TEST(URLValidatorTest, BlocksOctalEncodedIPs) {
  // SDL Test Case: Block octal IP encoding (0177.0.23.19 = 127.0.19.19)
  EXPECT_THROW(URLValidator::ValidateURL("https://0177.0.23.19/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://0200.0250.01.01/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksHexEncodedIPs) {
  // SDL Test Case: Block hex IP encoding (0x7f.00331.0246.174 = 127.x.x.x)
  EXPECT_THROW(URLValidator::ValidateURL("https://0x7f.00331.0246.174/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://0x7F.0x00.0x00.0x01/", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, BlocksDecimalEncodedIPs) {
  // SDL Test Case: Block decimal IP encoding (2130706433 = 127.0.0.1)
  EXPECT_THROW(URLValidator::ValidateURL("https://2130706433/", {"http", "https"}), ValidationException);
  EXPECT_THROW(URLValidator::ValidateURL("https://3232235777/", {"http", "https"}), ValidationException); // 192.168.1.1
}

TEST(URLValidatorTest, DecodesDoubleEncodedURLs) {
  // SDL Requirement: Decode URLs until no further decoding possible
  // %252e%252e = %2e%2e = .. (double encoded)
  EXPECT_THROW(
      URLValidator::ValidateURL("https://example.com/%252e%252e/etc/passwd", {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, EnforcesMaxLength) {
  // SDL: URL length limit (2048 bytes)
  std::string longURL = "https://example.com/" + std::string(3000, 'a');
  EXPECT_THROW(URLValidator::ValidateURL(longURL, {"http", "https"}), ValidationException);
}

TEST(URLValidatorTest, AllowsPublicURLs) {
  // Positive: Public URLs should work
  EXPECT_NO_THROW(URLValidator::ValidateURL("https://example.com/api/data", {"http", "https"}));
  EXPECT_NO_THROW(URLValidator::ValidateURL("http://192.0.2.1/", {"http", "https"})); // TEST-NET-1
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
  EXPECT_THROW(PathValidator::ValidateBlobId("blob@123"), ValidationException);
}

TEST(PathValidatorTest, BlobIDLengthLimit) {
  // SDL: Max 128 characters
  std::string validLength(128, 'a');
  EXPECT_NO_THROW(PathValidator::ValidateBlobId(validLength));

  std::string tooLong(129, 'a');
  EXPECT_THROW(PathValidator::ValidateBlobId(tooLong), ValidationException);
}

TEST(PathValidatorTest, FilePathAbsolutePathsBlocked) {
  // SDL: Absolute paths should be rejected
  EXPECT_THROW(PathValidator::ValidateFilePath("/etc/passwd", ""), ValidationException);
  EXPECT_THROW(PathValidator::ValidateFilePath("\\Windows\\System32", ""), ValidationException);
}

TEST(PathValidatorTest, FilePathDriveLettersBlocked) {
  // SDL: Drive letters should be rejected
  EXPECT_THROW(PathValidator::ValidateFilePath("C:\\Windows", ""), ValidationException);
  EXPECT_THROW(PathValidator::ValidateFilePath("D:/data", ""), ValidationException);
}

// ============================================================================
// SDL COMPLIANCE TESTS - Size Validation (DoS Prevention)
// ============================================================================

TEST(SizeValidatorTest, EnforcesMaxBlobSize) {
  // SDL: 100MB max
  EXPECT_NO_THROW(SizeValidator::ValidateSize(100 * 1024 * 1024, SizeValidator::MAX_BLOB_SIZE, "Blob"));
  EXPECT_THROW(
      SizeValidator::ValidateSize(101 * 1024 * 1024, SizeValidator::MAX_BLOB_SIZE, "Blob"), ValidationException);
}

TEST(SizeValidatorTest, EnforcesMaxWebSocketFrame) {
  // SDL: 256MB max
  EXPECT_NO_THROW(SizeValidator::ValidateSize(256 * 1024 * 1024, SizeValidator::MAX_WEBSOCKET_FRAME, "WebSocket"));
  EXPECT_THROW(
      SizeValidator::ValidateSize(257 * 1024 * 1024, SizeValidator::MAX_WEBSOCKET_FRAME, "WebSocket"),
      ValidationException);
}

TEST(SizeValidatorTest, EnforcesCloseReasonLimit) {
  // SDL: 123 bytes max (WebSocket spec)
  EXPECT_NO_THROW(SizeValidator::ValidateSize(123, SizeValidator::MAX_CLOSE_REASON, "Close reason"));
  EXPECT_THROW(SizeValidator::ValidateSize(124, SizeValidator::MAX_CLOSE_REASON, "Close reason"), ValidationException);
}

TEST(SizeValidatorTest, ValidatesInt32Range) {
  // SDL: Numeric range validation
  EXPECT_NO_THROW(SizeValidator::ValidateInt32Range(0, 0, 100, "Test"));
  EXPECT_NO_THROW(SizeValidator::ValidateInt32Range(50, 0, 100, "Test"));
  EXPECT_NO_THROW(SizeValidator::ValidateInt32Range(100, 0, 100, "Test"));

  EXPECT_THROW(SizeValidator::ValidateInt32Range(-1, 0, 100, "Test"), ValidationException);
  EXPECT_THROW(SizeValidator::ValidateInt32Range(101, 0, 100, "Test"), ValidationException);
}

TEST(SizeValidatorTest, ValidatesUInt32Range) {
  // SDL: Unsigned range validation
  EXPECT_NO_THROW(SizeValidator::ValidateUInt32Range(0, 0, 1000, "Test"));
  EXPECT_NO_THROW(SizeValidator::ValidateUInt32Range(1000, 0, 1000, "Test"));

  EXPECT_THROW(SizeValidator::ValidateUInt32Range(1001, 0, 1000, "Test"), ValidationException);
}

// ============================================================================
// SDL COMPLIANCE TESTS - Encoding Validation (CRLF Prevention)
// ============================================================================

TEST(EncodingValidatorTest, ValidBase64Format) {
  // Positive: Valid base64
  EXPECT_TRUE(EncodingValidator::IsValidBase64("SGVsbG8gV29ybGQ="));
  EXPECT_TRUE(EncodingValidator::IsValidBase64("YWJjZGVmZ2hpamtsbW5vcA=="));
}

TEST(EncodingValidatorTest, InvalidBase64Format) {
  // Negative: Invalid base64
  EXPECT_FALSE(EncodingValidator::IsValidBase64("Not@Valid!"));
  EXPECT_FALSE(EncodingValidator::IsValidBase64("abc")); // Wrong length (not multiple of 4)
  EXPECT_FALSE(EncodingValidator::IsValidBase64("")); // Empty
}

TEST(EncodingValidatorTest, DetectsCRLF) {
  // SDL Test Case: Detect CRLF injection
  EXPECT_TRUE(EncodingValidator::ContainsCRLF("Header: value\r\nInjected: malicious"));
  EXPECT_TRUE(EncodingValidator::ContainsCRLF("value\ninjected"));
  EXPECT_TRUE(EncodingValidator::ContainsCRLF("value\rinjected"));
}

TEST(EncodingValidatorTest, DetectsEncodedCRLF) {
  // SDL Test Case: Detect %0D%0A (encoded CRLF)
  EXPECT_TRUE(EncodingValidator::ContainsCRLF("value%0D%0Ainjected"));
  EXPECT_TRUE(EncodingValidator::ContainsCRLF("value%0d%0ainjected")); // lowercase
  EXPECT_TRUE(EncodingValidator::ContainsCRLF("value%0A")); // Just LF
}

TEST(EncodingValidatorTest, ValidHeaderValue) {
  // Positive: Valid headers
  EXPECT_NO_THROW(EncodingValidator::ValidateHeaderValue("application/json"));
  EXPECT_NO_THROW(EncodingValidator::ValidateHeaderValue("Bearer token123"));
  EXPECT_NO_THROW(EncodingValidator::ValidateHeaderValue("")); // Empty allowed
}

TEST(EncodingValidatorTest, InvalidHeaderWithCRLF) {
  // SDL Test Case: Block CRLF in headers
  EXPECT_THROW(EncodingValidator::ValidateHeaderValue("value\r\nX-Injected: evil"), ValidationException);
  EXPECT_THROW(EncodingValidator::ValidateHeaderValue("value%0D%0AX-Injected: evil"), ValidationException);
}

TEST(EncodingValidatorTest, HeaderLengthLimit) {
  // SDL: Header max 8KB
  std::string validHeader(8192, 'a');
  EXPECT_NO_THROW(EncodingValidator::ValidateHeaderValue(validHeader));

  std::string tooLong(8193, 'a');
  EXPECT_THROW(EncodingValidator::ValidateHeaderValue(tooLong), ValidationException);
}

// ============================================================================
// SDL COMPLIANCE TESTS - Logging
// ============================================================================

TEST(LoggingTest, LogsValidationFailures) {
  bool logged = false;
  std::string loggedCategory;
  std::string loggedMessage;

  SetValidationLogger([&](const std::string &category, const std::string &message) {
    logged = true;
    loggedCategory = category;
    loggedMessage = message;
  });

  // Trigger validation failure
  try {
    URLValidator::ValidateURL("https://localhost/", {"http", "https"});
  } catch (...) {
    // Expected
  }

  // Verify logging occurred
  EXPECT_TRUE(logged);
  EXPECT_EQ(loggedCategory, "SSRF_ATTEMPT");
  EXPECT_TRUE(loggedMessage.find("localhost") != std::string::npos);
}

// ============================================================================
// Run all tests
// ============================================================================

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
