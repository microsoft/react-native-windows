# React Native Windows creaternwapp Testing Guide

This document provides a comprehensive testing guide for validating React Native Windows creaternwapp functionality with both new architecture (Fabric) and old architecture (Paper/UWP).

## Prerequisites

1. Windows development environment with Visual Studio
2. Node.js and yarn installed
3. React Native Windows development dependencies
4. RNW_ROOT environment variable set (if using local development)

## Test Scenarios

### New Architecture (Fabric) Testing

#### Debug Mode
```bash
# Step 1: Create new app with Fabric architecture (replace VERSION with target version)
creaternwapp.cmd /rnw VERSION TestAppFabric

# Step 2: Navigate to app directory  
cd TestAppFabric

# Step 3: Start Metro bundler
yarn start

# Step 4: In a new terminal, build and run in debug mode
cd TestAppFabric
yarn windows
```

#### Release Mode
```bash
# Using the same app created above, build in release mode
cd TestAppFabric
yarn windows --release
```

### Old Architecture (Paper/UWP) Testing

#### Debug Mode
```bash
# Step 1: Create new app with UWP/Paper architecture (replace VERSION with target version)
creaternwapp.cmd /rnw VERSION /t old/uwp-cpp-app TestAppPaper

# Step 2: Navigate to app directory
cd TestAppPaper

# Step 3: Start Metro bundler
yarn start

# Step 4: In a new terminal, build and run in debug mode
cd TestAppPaper
yarn windows
```

#### Release Mode
```bash
# Using the same app created above, build in release mode
cd TestAppPaper
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

#### Paper/UWP (Old Architecture)
- [ ] Uses legacy Paper renderer
- [ ] UWP-specific features work correctly
- [ ] Compatible with existing UWP applications

## Expected Outcomes

1. **Successful App Creation**: Both architectures should create functional React Native Windows apps
2. **Version Consistency**: All apps should use the specified react-native-windows version
3. **Build Success**: Both debug and release builds should complete successfully  
4. **Runtime Functionality**: Apps should launch and display the default React Native screen

## Troubleshooting

### Common Issues
- **Build Failures**: Check Visual Studio components and Windows SDK versions
- **Version Conflicts**: Ensure clean environment and correct package versions
- **Network Issues**: May need to configure npm/yarn registries

### Debugging Steps
1. Check creaternwapp.cmd output for errors
2. Verify package.json dependencies
3. Review build logs for specific error messages
4. Ensure all prerequisites are met

## Automation Notes

This testing can be automated in CI/CD environments using:
- Windows-based build agents
- Verdaccio for package registry testing
- MSBuild for compilation validation
- Automated deployment testing

## Documentation

For additional information, see:
- React Native Windows documentation
- creaternwapp.cmd script source
- Template documentation in vnext/templates/