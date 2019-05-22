# Component & Module API Parity Status
This doc reports the status progress of core components modules/APIs integration for vnext. The high priority components and modules tracked here will be in alignment with the [React Native Lean Core](https://github.com/facebook/react-native/issues/23313) effort, others not in Lean Core we plan on shipping as separate packages in [Milestone 3](https://github.com/microsoft/react-native-windows/milestone/17).

Ones in **bold** are in the milestone we're currently targetting. If there is a discrepency here, or features are not being tracked that should be, please [submit and issue under vnext](https://github.com/microsoft/react-native-windows/issues/new?labels=vnext&template=vnext.md) to let us know.

## Lean Core Controls

We are currently in [**Milestone 2**](https://github.com/microsoft/react-native-windows/milestone/16).

### Components
Below are a list of components and their status towards parity between the lean core components you'd find on iOS and Android and the functionality we have today.

|Component| Status | Issues | C# Status |
|:-|:-|:-|:-|
|ActivityIndicator|**Complete**|-|**Complete**|
|Button|Partial|**[2182](https://github.com/microsoft/react-native-windows/issues/2182)**, **[2099](https://github.com/microsoft/react-native-windows/issues/2099)**, **[2085](https://github.com/microsoft/react-native-windows/issues/2085)** *(Completed for Button)*, [2163](https://github.com/microsoft/react-native-windows/issues/2163)|**Complete**|
|FlatList|Partial|**[2099](https://github.com/microsoft/react-native-windows/issues/2099)**|**Complete**|
|Image|Partial|**[2111](https://github.com/microsoft/react-native-windows/issues/2111)**|Partial|
|ScrollView|Partial|**[2113](https://github.com/microsoft/react-native-windows/issues/2113)**|Partial|
|Switch|Partial|[2161](https://github.com/microsoft/react-native-windows/issues/2161), [2140](https://github.com/microsoft/react-native-windows/issues/2140)|**Complete**|
|Text|Partial|[2484](https://github.com/microsoft/react-native-windows/issues/2484), [2162](https://github.com/microsoft/react-native-windows/issues/2162), [2121](https://github.com/microsoft/react-native-windows/issues/2121), [2161](https://github.com/microsoft/react-native-windows/issues/2161)|Partial|
|TextInput|Partial|**[2469](https://github.com/microsoft/react-native-windows/issues/2469)**, **[2198](https://github.com/microsoft/react-native-windows/issues/2198)**, **[2148](https://github.com/microsoft/react-native-windows/issues/2148)**, [2136](https://github.com/microsoft/react-native-windows/issues/2136)|Partial|
|View|Partial|**[2102](https://github.com/microsoft/react-native-windows/issues/2102)**, [2492](https://github.com/microsoft/react-native-windows/issues/2492), [2245](https://github.com/microsoft/react-native-windows/issues/2245), [2349](https://github.com/microsoft/react-native-windows/issues/2349), [2098](https://github.com/microsoft/react-native-windows/issues/2098)|**Complete**|
|VirtualizedList|Partial|No Issues Logged|**Complete**|

### Modules
Below are a list of modules and their status towards parity between the lean core components you'd find on iOS and Android and the functionality we have today.

Also tracked are NativeModules, as they are essential, even if not part of the core set.

|Module| Status | Issues | C# Status|
|:-|:-|:-|:-|
|Accessibility|Not Started|**[2088](https://github.com/microsoft/react-native-windows/issues/2088)**, [2326](https://github.com/microsoft/react-native-windows/issues/2326)|Partial|
|LayoutProps|**Completed**|[2143](https://github.com/Microsoft/react-native-windows/issues/2143)|Partial|
|Alert|Not Started|[2071](https://github.com/microsoft/react-native-windows/issues/2071)|**Complete**|
|AppState|Not Started|[2144](https://github.com/microsoft/react-native-windows/issues/2144)|**Complete**|
|Keyboard|Partial|**[2085](https://github.com/microsoft/react-native-windows/issues/2085)**, **[2469](https://github.com/microsoft/react-native-windows/issues/2469)**, **[2107](https://github.com/microsoft/react-native-windows/issues/2107)**, [2276](https://github.com/microsoft/react-native-windows/issues/2276)|Not Implemented|
|LayoutAnimation|Not Started|**[2315](https://github.com/microsoft/react-native-windows/issues/2315)**, **[2297](https://github.com/microsoft/react-native-windows/issues/2297)**, [2494](https://github.com/microsoft/react-native-windows/issues/2494), [2126](https://github.com/microsoft/react-native-windows/issues/2126)|Partial|
|Networking|Not Started|[2460](https://github.com/microsoft/react-native-windows/issues/2460)|**Complete**|
|NativeModules|Partial|[2393](https://github.com/microsoft/react-native-windows/issues/2393), [2388](https://github.com/microsoft/react-native-windows/issues/2388), [2387](https://github.com/microsoft/react-native-windows/issues/2387), [2271](https://github.com/microsoft/react-native-windows/issues/2271), [2130](https://github.com/microsoft/react-native-windows/issues/2130), [2103](https://github.com/microsoft/react-native-windows/issues/2103), [2104](https://github.com/microsoft/react-native-windows/issues/2104)|Partial|

## Packages to be Refactored Separately
These set of components and modules are packages we intend to ship separately but deliver ourselves. All of this work is scheduled for [Milestone 3](https://github.com/microsoft/react-native-windows/milestone/17).

### Components

|Component| Status | Issues | C# Status |
|:-|:-|:-|:-|
|Modal|Not Started|*nothing logged*|Partial (Beta)|
|Navigator|Not Started|*nothing logged*|**Complete**|
|Picker|Partial|*nothing logged*|Partial|
|RefreshControl|Not Started|*part of* [2113](https://github.com/microsoft/react-native-windows/issues/2113) *'s M3 effort*|Not Started|
|Slider|Not Started|*not logged*|**Complete**|
|StatusBar|Not Started|*not logged*|**Complete**|
|WebView|Partial|*not logged*|Partial|

### Modules

|Module| Status | Issues | C# Status|
|:-|:-|:-|:-|
|AsyncStorage|Partial|[2271](https://github.com/microsoft/react-native-windows/issues/2271)|**Complete**|
|BackAndroid|Partial|*nothing logged*|Partial|
|Clipboard|**Complete**|-|**Complete**|
|Geolocation|**Complete**|-|**Complete**|
|NetInfo|Not Started|*nothing logged*|Partial|
|AppRegistry|Not Started|*nothing logged*|**Complete**|
|NativeMethodsMixin|Not Started|*nothing logged*|**Complete**|
|PixelRatio|Not Started|*nothing logged*|**Complete**|
|Settings|Not Started|*nothing logged*|Not Started|
|ShadowProps|Not Started|*nothing logged*|Not Started|
|Timers|Not Started|*nothing logged*|**Complete**|
|Vibration|Not Started|*nothing logged*|**Complete**|
|Dimensions|Not Started|[2470](https://github.com/microsoft/react-native-windows/issues/2470)|**Complete**|
|Easing|Not Started|*nothing logged*|**Complete**|
