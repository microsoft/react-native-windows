# Component & Module API Parity Status
This doc reports the status progress of core components modules/APIs integration for vnext. The high priority components and modules tracked here will be in alignment with the [React Native Lean Core](https://github.com/facebook/react-native/issues/23313) effort, others not in Lean Core we plan on shipping as separate packages in [Milestone 3](https://github.com/microsoft/react-native-windows/milestone/17).

If there is a discrepency here, or features are not being tracked that should be, please [submit an issue under vnext](https://github.com/microsoft/react-native-windows/issues/new?labels=vnext&template=vnext.md) to let us know.

We are making steady progress in this workstream and will be investing more in [Milestone 3](https://github.com/microsoft/react-native-windows/milestone/17) with a goal to get to complete feature parity on lean-core components before EOY 2019. You can keep track of the progress here through the [Core Feature Parity project](https://github.com/microsoft/react-native-windows/projects/7).

### Components
Below are a list of components and their status towards parity between the lean core components you'd find on iOS and Android and the functionality we have today.

|Component| `vnext` version Status | `vnext` Issues remaining | `current` version Status |
|:-|:-|:-|:-|
|ActivityIndicator|**Complete**|-|**Complete**|
|Button|**Complete**|-|**Complete**|
|FlatList|Partial|[2795](https://github.com/microsoft/react-native-windows/issues/2795)|**Complete**|
|Image|Partial|[2111](https://github.com/microsoft/react-native-windows/issues/2111)|Partial|
|ScrollView|Partial|[2113](https://github.com/microsoft/react-native-windows/issues/2113)|Partial|
|Switch|Partial|[2140](https://github.com/microsoft/react-native-windows/issues/2140)|**Complete**|
|Text|Partial|[2162](https://github.com/microsoft/react-native-windows/issues/2162), [2121](https://github.com/microsoft/react-native-windows/issues/2121)|**Complete** *Partial - (Some properties not supported due to device platform differences*|
|TextInput|Partial|[2136](https://github.com/microsoft/react-native-windows/issues/2136)|Partial|
|View|Partial|[2796](https://github.com/microsoft/react-native-windows/issues/2796)|**Complete**|
|VirtualizedList|Partial|No Issues Logged|**Complete**|

### Modules
Below are a list of modules and their status towards parity between the lean core components you'd find on iOS and Android and the functionality we have today.

Also tracked are NativeModules, as they are essential, even if not part of the core set.

|Module| `vnext` version Status | `vnext` Issues remaining | `current` version Status|
|:-|:-|:-|:-|
|Accessibility|**Completed**|-|Partial|
|LayoutProps|**Completed**|-|Partial|
|Alert|**Completed**|-|**Complete**|
|AppState|Not Started|[2144](https://github.com/microsoft/react-native-windows/issues/2144)|**Complete**|
|Keyboard|Not Started|No Issues Logged|Not Implemented|
|LayoutAnimation|Not Started|[2494](https://github.com/microsoft/react-native-windows/issues/2494)|Partial|
|Networking|Not Started|[2460](https://github.com/microsoft/react-native-windows/issues/2460)|**Complete**|


## Packages to be Refactored Separately
These set of components and modules are not part of [React Native Lean Core](https://github.com/facebook/react-native/issues/23313) and hence will be moved into separate packages in React Native for Windows.

### Components

|Component| `vnext` version Status | `vnext` Issues remaining | `current` version Status |
|:-|:-|:-|:-|
|Modal|Not Started|*nothing logged*|Partial (Beta)|
|Navigator|Not Started|*nothing logged*|**Complete**|
|Picker|Partial|*nothing logged*|Partial|
|RefreshControl|Not Started|*part of* [2113](https://github.com/microsoft/react-native-windows/issues/2113) *'s M3 effort*|Not Started|
|Slider|Not Started|*not logged*|**Complete**|
|StatusBar|Not Started|*not logged*|**Complete**|
|WebView|Partial|*not logged*|Partial|

### Modules

|Module| `vnext` version Status | `vnext` Issues remaining | `current` version Status|
|:-|:-|:-|:-|
|AsyncStorage|Partial|[2271](https://github.com/microsoft/react-native-windows/issues/2271)|**Complete**|
|BackAndroid|Partial|*nothing logged*|Partial|
|Clipboard|**Complete**|-|**Complete**|
|Geolocation|**Complete**|-|**Complete**|
|NetInfo|Not Started|*nothing logged*|**Complete**|
|AppRegistry|Not Started|*nothing logged*|**Complete**|
|NativeMethodsMixin|Not Started|*nothing logged*|**Complete**|
|PixelRatio|Not Started|*nothing logged*|**Complete**|
|Settings|Not Started|*nothing logged*|Not Started|
|ShadowProps|Not Started|*nothing logged*|Not Started|
|Timers|Not Started|*nothing logged*|**Complete**|
|Vibration|Not Started|*nothing logged*|**Complete**|
|Dimensions|Not Started|[2470](https://github.com/microsoft/react-native-windows/issues/2470)|**Complete**|
|Easing|Not Started|*nothing logged*|**Complete**|
