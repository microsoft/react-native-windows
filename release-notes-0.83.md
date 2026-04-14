# React Native Windows 0.83

We're excited to release React Native Windows 0.83 targeting React Native 0.83!

This release includes the commits to React Native Windows from 01/15/2026 – 03/25/2026.

## New Architecture-specific changes

- Adds Text Component selectionColor support for Fabric: [Implements selectionColor for \<Text\> (#15553)](https://github.com/microsoft/react-native-windows/pull/15553)
- Added support for italic, underline, and strikethrough styling for TextInput: [Implements italic underline and strikethrough styling for TextInput (#15563)](https://github.com/microsoft/react-native-windows/pull/15563)
- Circular dependency build issues for RNW Fabric App fixed: [Fix Circular Dependency issues for New Arch Apps (#15747)](https://github.com/microsoft/react-native-windows/pull/15747)

## Breaking Changes

- Update WinAppSdk to 1.8.260209005: [Update WinAppSdk to 1.8.260209005 (#15660)](https://github.com/microsoft/react-native-windows/pull/15660)

## New Features

- Added `@react-native-windows/perf-testing`, a component performance testing framework for RNW Fabric: [\[Fabric\]: Performance Tests for React Native Windows (#15666)](https://github.com/microsoft/react-native-windows/pull/15666)
- Expose AutomationOption Setter for ChildSite, allowing other content islands connected in childsite to customize their automation option: [Expose AutomationOption Setter for ChildSite (#15622)](https://github.com/microsoft/react-native-windows/pull/15622)
- Adds support for detecting which mouse button triggered pointer events (left, right, middle, etc.) and registers missing W3C pointer event handlers: [Add button property mouse events (#15819)](https://github.com/microsoft/react-native-windows/pull/15819)

## Reliability

- Fix TextInput selection crash and feedback loop: [Fix TextInput selection prop (#15672)](https://github.com/microsoft/react-native-windows/pull/15672)
- Fixed SHIFT+F10 keyboard shortcut not showing context menu in TextInput components: [Fix SHIFT+F10 keyboard shortcut for context menu in TextInput (#15621)](https://github.com/microsoft/react-native-windows/pull/15621)
- Adding defensive check to ensure device context is valid before D2D operations: [adding defensive check to ensure device context is valid before D2D operations (#15620)](https://github.com/microsoft/react-native-windows/pull/15620)
- Fixed issue where box with corner radius doesn't have shadow: [Fixing box with corner radius doesn't have shadow (#15613)](https://github.com/microsoft/react-native-windows/pull/15613)
- Bring Narrator focus to the XAML island for community module native components: [Bring Narrator focus to XAML island (#15611)](https://github.com/microsoft/react-native-windows/pull/15611)
- Fixed selectable text not receiving pointer events inside ScrollView: [Fix selectable text not receiving pointer events inside ScrollView (#15577)](https://github.com/microsoft/react-native-windows/pull/15577)
- Fixed detection of projects with mixed architecture support: [fix(cli): fix template info not detecting mixed architecture projects (#15567)](https://github.com/microsoft/react-native-windows/pull/15567)
- Fixed a silent MSBuild linker bug in the cpp-app and cpp-lib templates where a typo in `%(AdditionalDependencies)` caused inherited link dependencies to be silently dropped: [fix: %(AdditionalDependencies) metadata typo in cpp-app/cpp-lib templates and package projects (#15720)](https://github.com/microsoft/react-native-windows/pull/15720)

## Other

- Integrate 0.84.0-nightly-20251212-dd390dbbe: [Integrate 0.84.0-nightly-20251212-dd390dbbe (#15807)](https://github.com/microsoft/react-native-windows/pull/15807)
- Integrate 0.83.0-nightly-20251104-502efe1cc: [Integrate 0.83.0-nightly-20251104-502efe1cc (#15691)](https://github.com/microsoft/react-native-windows/pull/15691)
- Integrate 0.83.0-nightly-20251012-6f482708b: [Integrate 0.83.0-nightly-20251012-6f482708b (#15667)](https://github.com/microsoft/react-native-windows/pull/15667)
