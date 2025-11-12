# React Native Windows creaternwapp Testing Guide

This document provides a comprehensive testing guide for validating React Native Windows creaternwapp functionality with both new architecture (Fabric) and old architecture (Paper/UWP).

## Prerequisites

1. Windows development environment with Visual Studio
2. Node.js and yarn installed
3. React Native Windows development dependencies
4. RNW_ROOT environment variable set (if using local development)

## Test Scenarios

The testing covers both stable and preview versions of React Native Windows:

- **Stable Version**: Tests the latest stable release from npm
- **Preview Version**: Tests the latest preview release from npm

### New Architecture (Fabric) Testing

#### Debug Mode
```bash
# Step 1: Create new app with Fabric architecture
# For stable version:
creaternwapp.cmd /rnw latest TestAppFabricStable
# For preview version:
creaternwapp.cmd /rnw preview TestAppFabricPreview

# Step 2: Navigate to app directory  
cd TestAppFabric*

# Step 3: Start Metro bundler
yarn start

# Step 4: In a new terminal, build and run in debug mode
cd TestAppFabric*
yarn windows
```

#### Release Mode
```bash
# Using the same app created above, build in release mode
cd TestAppFabric*
yarn windows --release
```

## Validation Checklist

### App Creation Validation
- [ ] App directory is created successfully
- [ ] package.json contains correct react-native-windows version
- [ ] windows directory is created with project files
- [ ] Solution (.sln) and project files are present
- [ ] Dependencies are installed correctly

### Build Validation  
- [ ] Debug build completes without errors
- [ ] Release build completes without errors
- [ ] App launches successfully
- [ ] No critical warnings in build output
- [ ] Bundle creation works (yarn start)

### Architecture-Specific Validation

#### Fabric (New Architecture)
- [ ] Uses modern Fabric renderer
- [ ] Supports new architecture features
- [ ] Compatible with latest React Native patterns

## Expected Outcomes

1. **Successful App Creation**: Both architectures should create functional React Native Windows apps for both stable and preview versions
2. **Version Consistency**: All apps should use the correct react-native-windows version (stable or preview as requested)
3. **Build Success**: Both debug and release builds should complete successfully  
4. **Runtime Functionality**: Apps should launch and display the default React Native screen

## Troubleshooting

### Common Issues
- **Build Failures**: Check Visual Studio components and Windows SDK versions
- **Version Conflicts**: Ensure clean environment and correct package versions
- **Network Issues**: May need to configure npm/yarn registries
- **Version Mismatch**: Verify npm registry has the expected stable/preview versions available

### Debugging Steps
1. Check creaternwapp.cmd output for errors
2. Verify package.json dependencies
3. Review build logs for specific error messages
4. Ensure all prerequisites are met
5. Verify npm registry connectivity and version availability

## Automation Notes

This testing can be automated in CI/CD environments using:
- Windows-based build agents
- Verdaccio for package registry testing
- MSBuild for compilation validation
- Automated deployment testing
- Npm registry queries to verify version availability

The automated tests now include:
- **Stable Version Tests**: Automatically fetch and test the latest stable version from npm
- **Preview Version Tests**: Automatically fetch and test the latest preview version from npm
- **Version Validation**: Verify that the fetched versions exist and have correct dependencies

## Documentation

For additional information, see:
- React Native Windows documentation
- creaternwapp.cmd script source
- Template documentation in vnext/templates/
- NPM package versions and tags for react-native-windows