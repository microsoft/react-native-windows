/* 
  RNTestApp exports a single ReactComponent, so react
  sees it as a Fast Refresh boundary.  If this module
  imports only RNTestApp, then all edits of any file
  will bubble up the refresh dependencies into
  RNTesterApp, which will be a Fast Refresh boundary,
  and all edits will be treated as Fast Refresh safe.
  Actually many edits in react-native core JS files are
  not safe for Fast Refresh.  In most apps, the root
  file imports react-native, which causes edits within
  react-native to reload the entire instance, unless its
  within a fast refresh boundary within react-native.
  To avoid these issues, we add a dependency edge by
  requiring react-native directly here in addition to
  RNTesterApp
*/
require('react-native');

function componentHasNativeconfig(name: string) {
  return !(global as unknown as {RN$Bridgeless:boolean}).RN$Bridgeless && name !== 'CustomXamlComponentWithNativeLayout' && name !== 'CustomXamlComponentWithYogaLayout'
}

const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
nativeComponentRegistry.setRuntimeConfigProvider((name: string) => {
  return {
    native: componentHasNativeconfig(name), // The fabric native component test has no viewmanager to get native config from
    strict: false,
    verify: componentHasNativeconfig(name),
  };
});

require('@react-native-windows/tester/js/RNTesterApp');

export {};
