# GitHub Copilot Instructions for React Native Windows

This file provides guidance to GitHub Copilot for contributing to the React Native Windows repository. React Native Windows brings React Native to the Windows platform, supporting both UWP and Win32 applications.

## Project Overview

React Native Windows is a Microsoft-maintained implementation of React Native that targets Windows platforms. The repository is a monorepo containing:

- **Native Windows implementation** (C++/C#) for React Native runtime
- **JavaScript/TypeScript tooling** for development experience
- **Sample applications** and test suites
- **Documentation** and build tools

### Key Architecture Components

- **Paper Architecture** (legacy): UWP XAML-based renderer
- **Fabric Architecture** (new): Win32 Composition-based renderer with WinAppSDK
- **Native Modules**: Platform-specific functionality exposed to JavaScript
- **View Managers**: Custom UI components
- **Turbo Modules**: New native module system with better performance

## Repository Structure

```
├── .github/           # GitHub configuration and templates
├── docs/             # Documentation
├── packages/         # Monorepo packages
│   ├── @react-native-windows/  # Core RNW packages
│   ├── @rnw-scripts/           # Build and development tools
│   └── sample-apps/            # Example applications
├── vnext/            # Main C++/C# implementation
└── tools/            # Development utilities
```

## Coding Standards and Conventions

### C++ Code (.cpp, .h files)

**Style Guide**: Follow the project's `.clang-format` configuration
- **Line length**: 120 characters maximum
- **Indentation**: 2 spaces, no tabs
- **Pointer alignment**: Right-aligned (`int *ptr`, not `int* ptr`)
- **Braces**: Attach style (same line as control statements)
- **Naming**: 
  - PascalCase for classes, methods, and properties
  - camelCase for local variables and parameters
  - UPPER_CASE for constants and macros

```cpp
// Good C++ style
class WindowsTextInput : public ViewManagerBase {
 public:
  WindowsTextInput(IReactContext const &reactContext);
  
  void SetText(std::wstring const &text) {
    m_text = text;
  }
  
 private:
  std::wstring m_text;
};
```

**Key Patterns**:
- Use `IReactContext` for accessing React Native context
- Prefer `winrt::` types for Windows Runtime interop
- Use `std::shared_ptr` for shared ownership
- Follow RAII principles for resource management

### C# Code (.cs files)

**Style Guide**: Follow Microsoft C# conventions
- **Indentation**: 4 spaces
- **Line endings**: CRLF (Windows style)
- **Naming**: PascalCase for public members, camelCase for private fields with underscore prefix

```csharp
// Good C# style
public sealed class ReactNativeModule
{
    private readonly IReactContext _reactContext;
    
    public ReactNativeModule(IReactContext reactContext)
    {
        _reactContext = reactContext ?? throw new ArgumentNullException("reactContext");
    }
    
    [ReactMethod]
    public void ShowAlert(string message)
    {
        // Implementation
    }
}
```

**Key Patterns**:
- Use `[ReactMethod]` attribute for methods exposed to JavaScript
- Use `[ReactProperty]` for bindable properties in view managers
- Prefer `sealed` classes for performance
- Use nullable reference types where appropriate

### TypeScript/JavaScript Code (.ts, .tsx, .js, .jsx files)

**Style Guide**: Follow Prettier configuration (`.prettierrc`)
- **Quotes**: Single quotes preferred
- **Semicolons**: Required
- **Trailing commas**: Always include
- **Indentation**: 2 spaces
- **Bracket spacing**: No spaces (`{foo}` not `{ foo }`)

```typescript
// Good TypeScript style
export interface WindowsSpecificProps {
  acceptsKeyboardFocus?: boolean;
  accessibilityRole?: string;
}

export const WindowsButton: React.FC<WindowsSpecificProps> = ({
  acceptsKeyboardFocus = true,
  accessibilityRole,
  children,
}) => {
  return (
    <Pressable
      acceptsKeyboardFocus={acceptsKeyboardFocus}
      accessibilityRole={accessibilityRole}>
      {children}
    </Pressable>
  );
};
```

**Key Patterns**:
- Use TypeScript for type safety
- Export interfaces for public APIs
- Prefer functional components with hooks
- Use React Native core components as base

## Development Workflow

### Building and Testing

```bash
# Install dependencies
yarn install

# Build all packages
yarn build

# Run linting
yarn lint

# Fix linting issues
yarn lint:fix

# Format C++ code
yarn format

# Run tests
yarn test
```

### Making Changes

1. **Create feature branch** from main
2. **Make minimal changes** following existing patterns
3. **Run linting and formatting** before committing
4. **Add change files** using `yarn change` for versioning
5. **Test thoroughly** including relevant test suites

### Native Module Development

**C++ Native Module Pattern**:
```cpp
REACT_MODULE(SampleModule)
struct SampleModule {
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;
  
  REACT_METHOD(GetDeviceName)
  void GetDeviceName(React::ReactPromise<std::string> promise) noexcept;
  
  REACT_EVENT(OnDataReceived)
  std::function<void(std::string)> OnDataReceived;
};
```

**C# Native Module Pattern**:
```csharp
[ReactModule]
public sealed class SampleModule
{
    [ReactMethod]
    public void GetDeviceName(IReactPromise<string> promise)
    {
        promise.Resolve(Environment.MachineName);
    }
    
    [ReactEvent]
    public Action<string> OnDataReceived { get; set; }
}
```

### View Manager Development

**C++ View Manager Pattern**:
```cpp
class SampleViewManager : public ViewManagerBase {
 public:
  SampleViewManager(IReactContext const &reactContext);
  
  const char *GetName() const override;
  winrt::FrameworkElement CreateView() noexcept override;
  
  void GetNativeProps(NativePropsBuilder const &builder) const override;
  void UpdateProperties(ShadowNodeBase const &shadowNode, winrt::FrameworkElement const &view) override;
};
```

## Common Patterns and Best Practices

### Error Handling
- Use `React::ReactPromise` for async operations that can fail
- Throw `React::ReactError` for immediate failures
- Log errors using `IReactContext::LogError()`

### Memory Management
- Use RAII and smart pointers in C++
- Avoid circular references between C++ and JavaScript
- Dispose of resources in component unmount/cleanup

### Performance Considerations
- Minimize cross-thread marshaling
- Use batch updates for multiple property changes
- Prefer native UI thread for UI operations
- Cache expensive computations

### Testing
- Write unit tests for business logic
- Use integration tests for React Native bridge interactions
- Include accessibility testing
- Test on both UWP and Win32 targets when applicable

## Technology-Specific Guidelines

### Windows Runtime (WinRT) Interop
- Use `winrt::` namespace for Windows Runtime types
- Handle exceptions from WinRT APIs appropriately
- Use `co_await` for async WinRT operations

### XAML Integration
- Follow XAML naming conventions for dependency properties
- Use data binding where appropriate
- Handle XAML events on UI thread

### Win32 Integration
- Use Windows App SDK APIs for modern Win32 features
- Handle Win32 messages appropriately
- Consider DPI awareness for UI elements

## File Organization

- **Source files**: Place in appropriate package under `packages/` or `vnext/`
- **Tests**: Co-locate with source or in dedicated test directories
- **Documentation**: Add to `docs/` for public APIs
- **Examples**: Include in sample applications when relevant

## Comments and Documentation

- Use JSDoc for TypeScript/JavaScript APIs
- Document public C++ APIs with doxygen-style comments
- Include usage examples for complex APIs
- Explain Windows-specific behavior differences

This guidance helps ensure consistent, high-quality contributions to React Native Windows while leveraging platform-specific capabilities effectively.