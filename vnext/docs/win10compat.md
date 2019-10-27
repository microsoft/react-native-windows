# Backward compatibility with Windows versions

React Native for Windows vnext supports Windows 10 devices and OS versions 10.0.15063 and higher. React Native for Windows layers cleanly on top of the [native Windows UI platform](https://github.com/microsoft/microsoft-ui-xaml) which means there are some APIs that may not be supported in all versions of Windows since the corresponding native capabilities were not backward compatible with older OS versions. The following table captures these deltas. If you are using any of the APIs below, they will gracefully fail (no-op) when the app is running on Windows 10 devices of corresponding OS versions. 

_Note : There is an ongoing effort to decouple the Windows UI platform from the UWP SDK and ship out of band – [WinUI 3.0](https://github.com/microsoft/microsoft-ui-xaml/blob/master/docs/roadmap.md#winui-3-q4-2019---2020). React Native for Windows vNext will layer on top of this lifted WinUI platform once it is available at which time several of the below unsupported APIs may start lighting up._ 

| [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive) | React Native for Windows vnext support |
|:-:|-|
| __May 2019 update__<br> Version-1903 ; Build-10.0.18362.1	| __All APIs, scenarios supported__ |
| __October 2018 update__<br> Version-1809 ; Build-10.0.17763.0 |	Unsupported: <ul><li>useNativeDriver for animations</li><li>View.transform property</li><li>React Native for Windows in XamlIslands for hosting inside WPF/Win32 scenarios</li>|
| __April 2018 Update__<br> Version-1803 ; Build-10.0.17134.12 | Unsupported : (Same as above plus): <ul><li>Flyout: edge alignments, ShowAt capability</li><li>Picker.editable, Picker.text</li> |
| __Fall Creators Update__<br> Version-1709 ; Build-10.0.16299.91 | Unsupported : Same as above |
| __Creators Update__<br> Version-1703 ; Build-10.0.15063.468 |	Unsupported : (Same as above plus): <ul><li>Keyboard events</li><li>TextInput.placeholderTextColor</li><li>TextInput.secureTextEntry may have some fast typing issues</li>
