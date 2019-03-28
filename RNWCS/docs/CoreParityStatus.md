**Disclaimer:** these tables are not necessarily complete or up-to-date. Feel free to add information on missing features, components, and modules as you encounter them.

## Components

| Name | Status | Notes |
|------|--------|-------------|
| ActivityIndicator | Ready |  |
| ARTGroup | Not Started |  |
| ARTShape | Not Started |  |
| ARTSurfaceView | Not Started |  |
| ARTText | Not Started |  |
| Button | Ready |  |
| DatePickerIOS | n/a | iOS-only. |
| DrawerLayoutAndroid | n/a | Android-only. See [SplitViewWindows](../ReactWindows/ReactNative/Views/Split/ReactSplitViewManager.cs). |
| Image | Partial | Missing tint color and background color ([#278](https://github.com/ReactWindows/react-native-windows/issues/278)), HTTP headers ([#689](https://github.com/ReactWindows/react-native-windows/issues/689)) |
| KeyboardAvoidingView | Not Started | Dependent on [#159](https://github.com/ReactWindows/react-native-windows/issues/159). |
| ListView | Ready | RecyclerBackedScrollViewManager unavailable ([#186](https://github.com/ReactWindows/react-native-windows/issues/186)). |
| MapView | n/a | Deprecated in favor of [react-native-maps](https://github.com/airbnb/react-native-maps). |
| Modal | Beta | See [#618](https://github.com/ReactWindows/react-native-windows/issues/618). |
| Navigator | Ready |  |
| NavigatorIOS | n/a | iOS-only. |
| Picker | Partial | Missing dialog-driven picker ([#535](https://github.com/ReactWindows/react-native-windows/issues/535)). |
| PickerIOS | n/a | iOS-only. |
| ProgressBarAndroid | n/a | Android-only. See [ProgressBarWindows](../ReactWindows/ReactNative/Views/Progress/ReactProgressBarViewManager.cs). |
| ProgressViewIOS | n/a | iOS-only. See [ProgressBarWindows](../ReactWindows/ReactNative/Views/Progress/ReactProgressBarViewManager.cs). |
| RefreshControl | Not Started | Consider building with [UWP Community Toolkit](https://github.com/Microsoft/UWPCommunityToolkit/tree/dev/Microsoft.Toolkit.Uwp.UI.Controls/PullToRefreshListView). |
| ScrollView | Partial | Missing momentum scroll events ([#534](https://github.com/ReactWindows/react-native-windows/issues/534)). |
| SegmentedControlIOS | n/a | iOS-only. |
| Slider | Ready |  |
| SnapshotViewIOS | n/a | iOS-only. |
| StatusBar | Ready |  |
| Switch | Ready |  |
| TabBarIOS | n/a | iOS-only. |
| TabBarIOS.Item | n/a | iOS-only. |
| Text | Partial | Issue with measurement performance ([#106](https://github.com/ReactWindows/react-native-windows/issues/106)). Also missing background color and borders ([#698](https://github.com/ReactWindows/react-native-windows/issues/698)) and strike-through and underlining ([#709](https://github.com/ReactWindows/react-native-windows/issues/709)). |
| TextInput | Partial | Missing rich text features ([#272](https://github.com/ReactWindows/react-native-windows/issues/272)) and auto-capitalize ([#801](https://github.com/ReactWindows/react-native-windows/issues/801)). |
| ToolbarAndroid | n/a | Android-only. |
| TouchableHighlight | Ready |  |
| TouchableNativeFeedback | Not Started | [#536](https://github.com/ReactWindows/react-native-windows/issues/536) |
| TouchableOpacity | Ready |  |
| TouchableWithoutFeedback | Ready |  |
| View | Ready |  |
| ViewPagerAndroid | n/a | Android-only. See [FlipViewWindows](../ReactWindows/ReactNative/Views/Flip/ReactFlipViewManager.cs). |
| WebView | Partial | Missing ability to set User-Agent string ([#537](https://github.com/ReactWindows/react-native-windows/issues/537)). |

## Modules

| Name | Status | Notes |
|------|--------|-------------|
| Accessibility | Partial | Supported: `accessibilityLiveRegion`, `accessibilityLabel`, `importantForAccessibility`, `accessibilityTraits`. Not supported by design: `accessible` - use `importantForAccessibility` instead. In `accessibilityTraits` only `button` and `none` are supported. See [#918](https://github.com/ReactWindows/react-native-windows/issues/918). |
| ActionSheetIOS | n/a | iOS-only. |
| AdSupportIOS | n/a | iOS-only. |
| Alert | Ready | See [Source](../ReactWindows/ReactNative/Modules/Dialog/DialogModule.cs). |
| AlertIOS | n/a | iOS-only. |
| Animated | Ready | See [Source](../ReactWindows/ReactNative/Modules/Core/Timing.cs). [NativeAnimated](https://github.com/ReactWindows/react-native-windows/tree/master/ReactWindows/ReactNative.Shared/Animated) also supported. |
| AppRegistry | Ready |  |
| AppState | Ready | See [Source](../ReactWindows/ReactNative/Modules/AppState/AppStateModule.cs). |
| AsyncStorage | Ready | See [Source](../ReactWindows/ReactNative/Modules/Storage/AsyncStorageModule.cs). |
| BackAndroid | Partial | Working, but intent is to create a Windows-specific version ([#538](https://github.com/ReactWindows/react-native-windows/issues/538)). |
| CameraRoll | Not Started | See [#183](https://github.com/ReactWindows/react-native-windows/issues/183). |
| Clipboard | Ready | See [Source](../ReactWindows/ReactNative/Modules/Clipboard/ClipboardModule.cs). |
| DatePickerAndroid | n/a | Android-only. |
| Dimensions | Ready |  |
| Easing | Ready |  |
| Geolocation | Ready | See [Source](../ReactWindows/ReactNative/Modules/Location/LocationModule.cs). |
| ImageEditor | Not Started |  |
| ImagePickerIOS | n/a | iOS-only. |
| ImageStore | Not Started |  |
| InteractionManager | Ready |  |
| Keyboard | Not Started | See [#159](https://github.com/ReactWindows/react-native-windows/issues/159). |
| LayoutAnimation | Partial | Missing [#175](https://github.com/ReactWindows/react-native-windows/issues/175), [#217](https://github.com/ReactWindows/react-native-windows/issues/217) |
| Layout Props | Partial | Working on integration of [facebook/css-layout](https://github.com/facebook/css-layout) |
| Linking | Ready |  |
| NativeMethodsMixin | Ready |  |
| NetInfo | Partial | Missing functionality to detect metered networks ([#539](https://github.com/ReactWindows/react-native-windows/issues/539)). |
| Networking | Ready | i.e., `fetch` and XMLHttpRequest |
| PanResponder | Partial | Missing functionality to block native responders ([#306](https://github.com/ReactWindows/react-native-windows/issues/306)). |
| PixelRatio | Ready |  |
| PushNotificationIOS | n/a | iOS-only. |
| Settings | Not Started |  |
| Shadow Props | Not Started |  |
| StatusBarIOS | n/a | iOS-only. |
| StyleSheet | Ready |  |
| TimePickerAndroid | n/a | Android-only. |
| ToastAndroid | n/a | Android-only. |
| Vibration | Ready | See [Source](../ReactWindows/ReactNative/Modules/Vibration/VibrationModule.cs). |
| VibrationIOS | n/a | iOS-only. |
| Timers | Partial | `set[Interval|Timeout]` complete, missing `requestIdleCallback` ([#917](https://github.com/ReactWindows/react-native-windows/issues/917)) |
