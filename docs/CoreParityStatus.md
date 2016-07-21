This is a very rough 

## Components

| Name | Status | Notes |
|------|--------|-------------|
| ActivityIndicatorIOS | n/a | iOS-only. |
| DatePickerIOS | n/a | iOS-only. |
| DrawerLayoutAndroid | n/a | Android-only. See [SplitViewWindows](../ReactWindows/ReactNative/Views/Split/ReactSplitViewManager.cs). |
| Image | Ready | [Missing prefetch (#366).](https://github.com/ReactWindows/react-native-windows/issues/366) |
| ListView | Ready | [RecyclerBackedScrollViewManager unavailable (#186).](https://github.com/ReactWindows/react-native-windows/issues/186) |
| MapView | Not Started |  |
| Modal | Not Started |  |
| Navigator | Ready |  |
| NavigatorIOS | n/a | iOS-only. |
| PickerIOS | n/a | iOS-only. |
| Picker | Partial | [Missing dialog-driven picker (#535).](https://github.com/ReactWindows/react-native-windows/issues/535) |
| ProgressBarAndroid | n/a | Android-only. See [ProgressBarWindows](../ReactWindows/ReactNative/Views/Progress/ReactProgressBarViewManager.cs). |
| ProgressViewIOS | n/a | iOS-only. See [ProgressBarWindows](../ReactWindows/ReactNative/Views/Progress/ReactProgressBarViewManager.cs). |
| RefreshControl | Not Started |  |
| ScrollView | Partial | [Missing momentum scroll events (#534).](https://github.com/ReactWindows/react-native-windows/issues/534) |
| SegmentedControlIOS | n/a | iOS-only. |
| Slider | Not Started |  |
| SliderIOS | n/a | iOS-only. |
| StatusBar | Ready |  |
| Switch | Ready |  |
| TabBarIOS | n/a | iOS-only. |
| TabBarIOS.Item | n/a | iOS-only. |
| Text | Ready | [Issue with measurement performance (#106).](https://github.com/ReactWindows/react-native-windows/issues/106) |
| TextInput | Partial | [Missing rich text features (#272).](https://github.com/ReactWindows/react-native-windows/issues/272) |
| ToolbarAndroid | n/a | Android-only. |
| TouchableHighlight | Ready |  |
| TouchableNativeFeedback | Not Started | [#536](https://github.com/ReactWindows/react-native-windows/issues/536) |
| TouchableOpacity | Ready |  |
| TouchableWithoutFeedback | Ready |  |
| View | Ready |  |
| ViewPagerAndroid | n/a | Android-only. See [FlipViewWindows](../ReactWindows/ReactNative/Views/Flip/ReactFlipViewManager.cs). |
| WebView | Partial | [Missing ability to set User-Agent string (#537).](https://github.com/ReactWindows/react-native-windows/issues/537) |

## Modules

| Name | Status | Notes |
|------|--------|-------------|
| ActionSheetIOS | n/a | iOS-only. |
| Alert | Ready | See [Source](../ReactWindows/ReactNative/Modules/Dialog/DialogModule.cs). |
| AlertIOS | n/a | iOS-only. |
| Animated | Ready | See [Source](../ReactWindows/ReactNative/Modules/Core/Timing.cs). |
| AppRegistry | Ready |  |
| AppStateIOS | n/a | iOS-only. |
| AppState | Ready | See [Source](../ReactWindows/ReactNative/Modules/AppState/AppStateModule.cs). |
| AsyncStorage | Ready | See [Source](../ReactWindows/ReactNative/Modules/Storage/AsyncStorageModule.cs). |
| BackAndroid | Partial | [Working, but intent is to create a Windows-specific version (#538).](https://github.com/ReactWindows/react-native-windows/issues/538) |
| CameraRoll | Not Started |  |
| Clipboard | Ready | See [Source](../ReactWindows/ReactNative/Modules/Clipboard/ClipboardModule.cs). |
| DatePickerAndroid | n/a | Android-only. |
| Dimensions | Ready |  |
| IntentAndroid | n/a | Android-only. |
| InteractionManager | Ready |  |
| LayoutAnimation | Partial | Missing [#175](https://github.com/ReactWindows/react-native-windows/issues/175), [#217](https://github.com/ReactWindows/react-native-windows/issues/217), [#508](https://github.com/ReactWindows/react-native-windows/issues/508) |
| Linking | Partial | [Missing functionality to get initial (i.e., launch) URI (#301).](https://github.com/ReactWindows/react-native-windows/issues/301) |
| LinkingIOS | n/a | iOS-only. |
| NativeMethodsMixin | Ready |  |
| NetInfo | Partial | [Missing functionality to detect metered networks (#539).](https://github.com/ReactWindows/react-native-windows/issues/539) |
| PanResponder | Partial | [Missing functionality to block native responders (#306).](https://github.com/ReactWindows/react-native-windows/issues/306) |
| PixelRatio | Not Started | [#540](https://github.com/ReactWindows/react-native-windows/issues/540) |
| PushNotificationIOS | n/a | iOS-only. |
| StatusBarIOS | n/a | iOS-only. |
| StyleSheet | Ready |  |
| TimePickerAndroid | n/a | Android-only. |
| ToastAndroid | n/a | Android-only. |
| VibrationIOS | n/a | iOS-only. |
| Vibration | Ready | See [Source](../ReactWindows/ReactNative/Modules/Vibration/VibrationModule.cs). |

## Polyfills

| Name | Status | Notes |
|------|--------|-------------|
| Flexbox | Ready |  |
| ShadowPropTypesIOS | n/a | iOS-only. |
| Geolocation | Ready |  See [Source](../ReactWindows/ReactNative/Modules/Location/LocationModule.cs). |
| Network | Ready | See [Source](../ReactWindows/ReactNative/Modules/Network/NetworkingModule.cs).  |
| Timers | Ready | See [Source](../ReactWindows/ReactNative/Modules/Core/Timing.cs). |
| Colors | Ready |  |
