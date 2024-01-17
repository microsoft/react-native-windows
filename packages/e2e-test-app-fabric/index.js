require('react-native');

function componentHasNativeconfig(name: string) {
  return name !== 'MyCustomComponent' && name !== 'MyCustomComponentYoga'
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