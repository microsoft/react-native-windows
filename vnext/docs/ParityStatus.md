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
|FlatList|**Complete**|-|**Complete**|
|Image|Partial|[Image project board](https://github.com/microsoft/react-native-windows/projects/18)|Partial|
|ScrollView|Partial|[ScrollView project board](https://github.com/microsoft/react-native-windows/projects/17)|Partial|
|Switch|**Complete**|-|**Complete**|
|Text|**Complete** *(Some properties not supported due to device platform differences, docs coming soon)*|-|**Complete**|
|TextInput|Partial|[TextInput project board](https://github.com/microsoft/react-native-windows/projects/20)|Partial|
|KeyboardAvoidingView|**Complete**|-|Not supported|
|View|Partial (some Style props left)|[View Style props project board](https://github.com/microsoft/react-native-windows/projects/19)|**Complete**|
|VirtualizedList|Partial|No Issues Logged|**Complete**|

### Modules
Below are a list of modules and their status towards parity between the lean core components you'd find on iOS and Android and the functionality we have today.

Also tracked are NativeModules, as they are essential, even if not part of the core set.

|Module| `vnext` version Status | `vnext` Issues remaining | `current` version Status|
|:-|:-|:-|:-|
|Accessibility|Partial|[Accessibility project board](https://github.com/microsoft/react-native-windows/projects/21)|Partial|
|LayoutProps|**Completed**|-|Partial|
|Alert|**Completed**|-|**Complete**|
|AppState|Partial|[2144](https://github.com/microsoft/react-native-windows/issues/2144)|**Complete**|
|Keyboard|**Complete** *(keyboardWillShow/WillHide, keyboardWillChangeFrame/DidChangeFrame will no-op in Windows)*|-|Not Implemented|
|Linking|Partial|[2853](https://github.com/microsoft/react-native-windows/issues/2853)|Partial|
|LayoutAnimation|Not Started|[2494](https://github.com/microsoft/react-native-windows/issues/2494)|Partial|
|Networking|Partial|[2460](https://github.com/microsoft/react-native-windows/issues/2460), [3178](https://github.com/microsoft/react-native-windows/issues/3178)|**Complete**|


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
|Timers|Not Started|*nothing logged*|**Complete**|
|Vibration|Not Started|*nothing logged*|**Complete**|
|Dimensions|Not Started|[2470](https://github.com/microsoft/react-native-windows/issues/2470)|**Complete**|
|Easing|Not Started|*nothing logged*|**Complete**|
