**Disclaimer:** these tables are not necessarily complete or up-to-date. Feel free to add information on missing features, components, and modules as you encounter them.

## View Props

| Component | Name | Status | Notes |
|-----------|------|--------|-------|
| View | Accessibility | Partial | Supported: `accessibilityLiveRegion`, `accessibilityLabel`, `importantForAccessibility`, `accessibilityTraits`. Not supported by design: `accessible` - use `importantForAccessibility` instead. In `accessibilityTraits` only `button` and `none` are supported. See [#918](https://github.com/ReactWindows/react-native-windows/issues/918). |
| View | shadow* | Not Started | TODO: add description |


## Components

| Name | Status | Notes |
|------|--------|-------------|
| ActivityIndicator | Ready |  |
| ART | Not Started | Includes `ARTGroup`, `ARTShape`, `ARTSurfaceView`, `ARTText` |
| Button | Ready |  |
| CheckBox | Not Started | |
| DatePickerIOS | n/a | iOS-only. |
| DrawerLayoutAndroid | n/a | Android-only. See [SplitViewWindows](../ReactWindows/ReactNative/Views/Split/ReactSplitViewManager.cs). |
| FlatList | TBD | |
| Image | Partial | Missing tint color and background color ([#278](https://github.com/ReactWindows/react-native-windows/issues/278)), HTTP headers ([#689](https://github.com/ReactWindows/react-native-windows/issues/689)) |
| ImageBackground | TBD |  |
| InputAccessoryView | n/a | iOS-only |
| KeyboardAvoidingView | Not Started | Dependent on [#159](https://github.com/ReactWindows/react-native-windows/issues/159). |
| ListView | Ready | RecyclerBackedScrollViewManager unavailable ([#186](https://github.com/ReactWindows/react-native-windows/issues/186)). |
| MaskedViewIOS | n/a | iOS-only. |
| Modal | Not Started | See [#618](https://github.com/ReactWindows/react-native-windows/issues/618). |
| NavigatorIOS | n/a | iOS-only. |
| Picker | Partial | Missing dialog-driven picker ([#535](https://github.com/ReactWindows/react-native-windows/issues/535)). |
| PickerIOS | n/a | iOS-only. |
| ProgressBarAndroid | n/a | Android-only. See [ProgressBarWindows](../ReactWindows/ReactNative/Views/Progress/ReactProgressBarViewManager.cs). |
| ProgressViewIOS | n/a | iOS-only. See [ProgressBarWindows](../ReactWindows/ReactNative/Views/Progress/ReactProgressBarViewManager.cs). |
| RefreshControl | Not Started | Consider building with [UWP Community Toolkit](https://github.com/Microsoft/UWPCommunityToolkit/tree/dev/Microsoft.Toolkit.Uwp.UI.Controls/PullToRefreshListView). |
| SafeAreaView | n/a | iOS-only. |
| ScrollView | Partial | Missing momentum scroll events ([#534](https://github.com/ReactWindows/react-native-windows/issues/534)) and sticky headers ([#1242](https://github.com/Microsoft/react-native-windows/issues/1242)). |
| SectionList | TBD |  |
| SegmentedControlIOS | n/a | iOS-only. |
| Slider | Ready |  |
| SnapshotViewIOS | n/a | iOS-only. |
| StatusBar | Ready |  |
| SwipeableFlatList | TBD |  |
| SwipeableListView | Ready | Requires `manipulationMode` prop setting. |
| Switch | Ready |  |
| TabBarIOS | n/a | iOS-only. |
| TabBarIOS.Item | n/a | iOS-only. |
| Text | Ready | Missing background color and borders ([#698](https://github.com/ReactWindows/react-native-windows/issues/698)) and strike-through and underlining ([#709](https://github.com/ReactWindows/react-native-windows/issues/709)). Remaining issues TBD. |
| TextInput | Ready | Missing rich text features ([#272](https://github.com/ReactWindows/react-native-windows/issues/272)) and auto-capitalize ([#801](https://github.com/ReactWindows/react-native-windows/issues/801)). Remaining issues TBD. |
| ToolbarAndroid | n/a | Android-only. |
| Touchable | Ready |  |
| TouchableHighlight | Ready |  |
| TouchableNativeFeedback | n/a | Android-only. |
| TouchableOpacity | Ready |  |
| TouchableWithoutFeedback | Ready |  |
| View | Ready |  |
| ViewPagerAndroid | n/a | Android-only. See [FlipViewWindows](../ReactWindows/ReactNative/Views/Flip/ReactFlipViewManager.cs). |
| VirtualizedList | TBD |  |
| WebView | Ready | Missing ability to set User-Agent string ([#537](https://github.com/ReactWindows/react-native-windows/issues/537)). Remaining issues TBD. |

## Modules

| Name | Status | Notes |
|------|--------|-------|
| AccessibilityInfo | Partial |  |
| ActionSheetIOS | n/a | iOS-only. |
| Alert | Ready | See [Source](../ReactWindows/ReactNative/Modules/Dialog/DialogModule.cs). |
| AlertIOS | n/a | iOS-only. |
| Animated | Ready | [NativeAnimated](https://github.com/ReactWindows/react-native-windows/tree/master/ReactWindows/ReactNative.Shared/Animated) also supported. |
| AppRegistry | Ready |  |
| AppState | Ready | See [Source](../ReactWindows/ReactNative/Modules/AppState/AppStateModule.cs). |
| AsyncStorage | Ready | See [Source](../ReactWindows/ReactNative/Modules/Storage/AsyncStorageModule.cs). |
| BackAndroid | Ready | Deprecated in favor of `BackHandler`. |
| BackHandler | Ready |  |
| CameraRoll | Not Started | See [#183](https://github.com/ReactWindows/react-native-windows/issues/183). |
| Clipboard | Ready | See [Source](../ReactWindows/ReactNative/Modules/Clipboard/ClipboardModule.cs). |
| DatePickerAndroid | n/a | Android-only. |
| DeviceInfo | TBD |  |
| Dimensions | Ready |  |
| Easing | Ready |  |
| I18NManager | TBD |  |
| ImageEditor | Not Started |  |
| ImagePickerIOS | n/a | iOS-only. |
| ImageStore | Not Started |  |
| InteractionManager | Ready |  |
| Keyboard | Not Started | See [#159](https://github.com/ReactWindows/react-native-windows/issues/159). |
| LayoutAnimation | Partial | Missing [#175](https://github.com/ReactWindows/react-native-windows/issues/175), [#217](https://github.com/ReactWindows/react-native-windows/issues/217) |
| Linking | Ready |  |
| NetInfo | Partial | Missing functionality to detect metered networks ([#539](https://github.com/ReactWindows/react-native-windows/issues/539)). |
| PanResponder | Ready |  |
| PermissionsAndroid | n/a | Android-only. |
| PixelRatio | Ready |  |
| PushNotificationIOS | n/a | iOS-only. |
| Settings | Not Started |  |
| Share | Not Started |  |
| StatusBarIOS | n/a | iOS-only. |
| StyleSheet | Ready |  |
| TimePickerAndroid | n/a | Android-only. |
| Timers | Partial | `set[Interval|Timeout]` complete, missing `requestIdleCallback` ([#917](https://github.com/ReactWindows/react-native-windows/issues/917)) |
| ToastAndroid | n/a | Android-only. |
| TVEventHandler | n/a | tvOS-only. |
| UIManager | Ready | TODO: add missing details |
| Vibration | Ready | See [Source](../ReactWindows/ReactNative/Modules/Vibration/VibrationModule.cs). |
| VibrationIOS | n/a | iOS-only. |
| YellowBox | Ready | TODO: add missing details |

## Plugins

| Name | Status | Notes |
|------|--------|-------|
| takeSnapshot | Not Started |  |

## Polyfills

| Name | Status | Notes |
|------|--------|-------|
| Blob | Not Started | See [#1471](https://github.com/Microsoft/react-native-windows/issues/1471). |
| Geolocation | Ready | See [Source](../ReactWindows/ReactNative/Modules/Location/LocationModule.cs). |
| Networking | Ready | i.e., `fetch` and XMLHttpRequest |

## Developer Experience

| Name | Status | Notes |
|------|--------|-------|
| Bundle Progress | Not Started |  |
| Delta Bundle | Not Started |  |
| Dev Support Menu | Ready | Use `Shift+F10` or `Menu` key, or shake device. |
| Hot Module Reloading | Ready |  |
| Live Reloading | Ready | TODO: add details. |
| Manual Reloading | Ready | Use `Ctrl+R`. |
| Remote Debugging | Ready | Chrome-only, no VS Code support. |
