# Security Best Practices for React Native Windows

This document outlines security best practices for developing applications with React Native Windows, focusing on secure coding patterns, API usage, and common security pitfalls to avoid.

## Table of Contents

- [Secure Development Practices](#secure-development-practices)
- [Native Module Security](#native-module-security)
- [WebView Security](#webview-security)
- [Data Protection](#data-protection)
- [Network Security](#network-security)
- [Common Security Pitfalls](#common-security-pitfalls)

## Secure Development Practices

### Input Validation

Always validate inputs in native modules and JavaScript code:

```cpp
// Native module input validation example
REACT_METHOD(ProcessUserInput)
void ProcessUserInput(std::string userInput) noexcept {
    // Validate input length
    if (userInput.length() > MAX_INPUT_LENGTH) {
        // Log security event and reject
        return;
    }
    
    // Sanitize input
    std::string sanitized = SanitizeInput(userInput);
    
    // Process sanitized input
    ProcessSafeInput(sanitized);
}
```

```javascript
// JavaScript input validation
function validateUserInput(input) {
    if (typeof input !== 'string' || input.length > 1000) {
        throw new Error('Invalid input');
    }
    
    // Remove potentially dangerous characters
    return input.replace(/[<>\"'&]/g, '');
}
```

### Memory Management

Use RAII and smart pointers in native code:

```cpp
#include <memory>

class SecureNativeModule {
public:
    void ProcessData(const std::vector<uint8_t>& data) {
        // Use smart pointers for automatic cleanup
        auto buffer = std::make_unique<uint8_t[]>(data.size());
        
        // Process data safely
        std::copy(data.begin(), data.end(), buffer.get());
        
        // Automatic cleanup when scope exits
    }
};
```

### Error Handling

Implement secure error handling that doesn't leak sensitive information:

```cpp
REACT_METHOD(SecureOperation)
void SecureOperation(std::string input, ReactPromise<std::string> promise) noexcept {
    try {
        auto result = PerformSecureOperation(input);
        promise.Resolve(result);
    }
    catch (const SecurityException& e) {
        // Log detailed error internally
        LogSecurityEvent(e.what());
        
        // Return generic error to client
        promise.Reject("Operation failed");
    }
    catch (const std::exception& e) {
        // Log error and return generic message
        LogError(e.what());
        promise.Reject("An error occurred");
    }
}
```

## Native Module Security

### Windows Security API Integration

Use Windows security APIs through native modules:

```cpp
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Storage::Streams;

REACT_METHOD(GenerateSecureRandom)
void GenerateSecureRandom(int length, ReactPromise<std::string> promise) noexcept {
    try {
        // Use Windows cryptographically secure random generation
        auto buffer = CryptographicBuffer::GenerateRandom(length);
        auto base64 = CryptographicBuffer::EncodeToBase64String(buffer);
        
        promise.Resolve(winrt::to_string(base64));
    }
    catch (...) {
        promise.Reject("Failed to generate random data");
    }
}
```

### Credential Management

Implement secure credential handling:

```cpp
#include <winrt/Windows.Security.Credentials.h>

using namespace winrt::Windows::Security::Credentials;

REACT_METHOD(StoreCredential)
void StoreCredential(std::string resource, std::string userName, 
                    std::string password, ReactPromise<bool> promise) noexcept {
    try {
        PasswordVault vault;
        PasswordCredential credential(
            winrt::to_hstring(resource),
            winrt::to_hstring(userName), 
            winrt::to_hstring(password)
        );
        
        vault.Add(credential);
        promise.Resolve(true);
    }
    catch (...) {
        promise.Reject("Failed to store credential");
    }
}
```

## WebView Security

### Secure WebView Configuration

Configure WebView2 with appropriate security settings:

```cpp
// WebView2 security configuration
auto webviewOptions = WebView2::CoreWebView2CreationProperties();
webviewOptions.put_AdditionalBrowserArguments(
    L"--disable-web-security=false "
    L"--enable-features=msWebView2BrowserHitTesting "
    L"--force-effective-connection-type=4g"
);

// Configure content security
webview.NavigationStarting([](auto&&, auto&& args) {
    auto uri = args.Uri();
    
    // Validate allowed origins
    if (!IsAllowedOrigin(uri)) {
        args.Cancel(true);
    }
});
```

### Content Security Policy

Implement Content Security Policy for web content:

```html
<!-- Include CSP headers in web content -->
<meta http-equiv="Content-Security-Policy" 
      content="default-src 'self'; 
               script-src 'self' 'unsafe-inline'; 
               style-src 'self' 'unsafe-inline';
               img-src 'self' data: https:;
               connect-src 'self' https:;">
```

### Origin Validation

Validate message origins in WebView communication:

```javascript
// JavaScript side - validate message origin
window.addEventListener('message', (event) => {
    // Validate origin
    if (event.origin !== 'https://trusted-domain.com') {
        return;
    }
    
    // Process trusted message
    handleTrustedMessage(event.data);
});
```

```cpp
// Native side - validate WebView messages
webview.WebMessageReceived([](auto&&, auto&& args) {
    auto message = args.TryGetWebMessageAsString();
    
    // Validate message format and content
    if (!IsValidMessage(message)) {
        return;
    }
    
    ProcessValidMessage(message);
});
```

## Data Protection

### Encryption

Use Windows encryption APIs for sensitive data:

```cpp
#include <winrt/Windows.Security.Cryptography.Core.h>

using namespace winrt::Windows::Security::Cryptography::Core;

REACT_METHOD(EncryptData)
void EncryptData(std::string plaintext, ReactPromise<std::string> promise) noexcept {
    try {
        // Use AES encryption
        auto algorithm = SymmetricKeyAlgorithmProvider::OpenAlgorithm(
            SymmetricAlgorithmNames::AesGcm());
        
        auto key = algorithm.GenerateKey(256 / 8); // 256-bit key
        
        auto plaintextBuffer = CryptographicBuffer::ConvertStringToBinary(
            winrt::to_hstring(plaintext), BinaryStringEncoding::Utf8);
        
        auto encryptedBuffer = CryptographicEngine::Encrypt(
            key, plaintextBuffer, nullptr);
        
        auto base64 = CryptographicBuffer::EncodeToBase64String(encryptedBuffer);
        promise.Resolve(winrt::to_string(base64));
    }
    catch (...) {
        promise.Reject("Encryption failed");
    }
}
```

### Secure Storage

Implement secure local storage:

```javascript
import {NativeModules} from 'react-native';
const {SecureStorage} = NativeModules;

class SecureDataManager {
    static async storeSecureData(key, data) {
        try {
            // Encrypt data before storage
            const encrypted = await SecureStorage.encrypt(JSON.stringify(data));
            await SecureStorage.store(key, encrypted);
        } catch (error) {
            console.error('Failed to store secure data:', error);
            throw error;
        }
    }
    
    static async retrieveSecureData(key) {
        try {
            const encrypted = await SecureStorage.retrieve(key);
            if (!encrypted) return null;
            
            const decrypted = await SecureStorage.decrypt(encrypted);
            return JSON.parse(decrypted);
        } catch (error) {
            console.error('Failed to retrieve secure data:', error);
            throw error;
        }
    }
}
```

## Network Security

### TLS Configuration

Ensure proper TLS configuration for network requests:

```javascript
// Use HTTPS for all network requests
const API_BASE_URL = 'https://api.example.com';

class SecureNetworking {
    static async makeSecureRequest(endpoint, options = {}) {
        const url = `${API_BASE_URL}${endpoint}`;
        
        const secureOptions = {
            ...options,
            // Ensure HTTPS
            method: options.method || 'GET',
            headers: {
                'Content-Type': 'application/json',
                ...options.headers,
            },
        };
        
        // Add request validation
        if (!url.startsWith('https://')) {
            throw new Error('Only HTTPS requests are allowed');
        }
        
        try {
            const response = await fetch(url, secureOptions);
            
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }
            
            return await response.json();
        } catch (error) {
            console.error('Network request failed:', error);
            throw error;
        }
    }
}
```

### Certificate Pinning

Implement certificate pinning for critical connections:

```cpp
// Certificate pinning in native module
REACT_METHOD(MakePinnedRequest)
void MakePinnedRequest(std::string url, ReactPromise<std::string> promise) noexcept {
    try {
        // Validate certificate against pinned certificate
        if (!ValidateCertificatePinning(url)) {
            promise.Reject("Certificate pinning validation failed");
            return;
        }
        
        // Make secure request
        auto result = MakeSecureHttpRequest(url);
        promise.Resolve(result);
    }
    catch (...) {
        promise.Reject("Request failed");
    }
}
```

## Common Security Pitfalls

### Avoid These Patterns

#### ❌ Insecure Random Number Generation
```javascript
// Don't use Math.random() for security-sensitive operations
const insecureToken = Math.random().toString(36);
```

#### ✅ Use Cryptographically Secure Random
```javascript
import {NativeModules} from 'react-native';
const {WindowsSecurity} = NativeModules;

const secureToken = await WindowsSecurity.generateSecureRandom(32);
```

#### ❌ Storing Sensitive Data in Plain Text
```javascript
// Don't store sensitive data unencrypted
AsyncStorage.setItem('password', plainTextPassword);
```

#### ✅ Use Secure Storage
```javascript
// Use encrypted storage for sensitive data
await SecureDataManager.storeSecureData('credentials', {
    username,
    password
});
```

#### ❌ Insufficient Input Validation
```cpp
// Don't trust user input
void ProcessInput(std::string input) {
    // Direct usage without validation - DANGEROUS
    ExecuteCommand(input);
}
```

#### ✅ Comprehensive Input Validation
```cpp
// Validate and sanitize all inputs
void ProcessInput(std::string input) {
    if (!IsValidInput(input)) {
        throw SecurityException("Invalid input");
    }
    
    auto sanitized = SanitizeInput(input);
    ExecuteCommand(sanitized);
}
```

### Security Testing

#### Unit Tests for Security Functions
```cpp
// Test security functions
TEST_CLASS(SecurityTests) {
public:
    TEST_METHOD(TestInputValidation) {
        // Test with valid input
        ASSERT_TRUE(IsValidInput("normal_input"));
        
        // Test with malicious input
        ASSERT_FALSE(IsValidInput("<script>alert('xss')</script>"));
        ASSERT_FALSE(IsValidInput("'; DROP TABLE users; --"));
    }
    
    TEST_METHOD(TestEncryption) {
        std::string plaintext = "sensitive data";
        auto encrypted = EncryptData(plaintext);
        auto decrypted = DecryptData(encrypted);
        
        ASSERT_EQ(plaintext, decrypted);
        ASSERT_NE(plaintext, encrypted);
    }
};
```

### Security Monitoring

#### Logging Security Events
```cpp
class SecurityLogger {
public:
    static void LogSecurityEvent(const std::string& event, 
                                const std::string& details) {
        // Log to Windows Event Log
        auto eventLog = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView()
            .CoreWindow().Dispatcher();
        
        // Log security events for monitoring
        OutputDebugStringA(("SECURITY: " + event + " - " + details).c_str());
    }
};
```

## Security Checklist

### Development Phase
- [ ] Input validation implemented for all user inputs
- [ ] Memory management uses RAII and smart pointers  
- [ ] Error handling doesn't leak sensitive information
- [ ] Cryptographically secure random number generation
- [ ] Sensitive data encrypted before storage

### Testing Phase
- [ ] Security unit tests implemented
- [ ] Input validation tests with malicious inputs
- [ ] Encryption/decryption functionality tested
- [ ] Certificate validation tested
- [ ] WebView security configuration validated

### Deployment Phase
- [ ] HTTPS enforced for all network communications
- [ ] Certificate pinning implemented for critical services
- [ ] Content Security Policy configured for WebViews
- [ ] Sensitive information removed from production builds
- [ ] Security logging and monitoring enabled

---

**Document Version**: 1.0  
**Last Updated**: October 14, 2025  
**Related**: [Security Configuration Guide](security-configuration.md)