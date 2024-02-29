
// This turns on validation that the ViewManager viewconfigs align with the JS ViewConfigs.
const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
nativeComponentRegistry.setRuntimeConfigProvider((name: string) => {
  return {
    native: true,
    strict: false,
    verify: name !== 'WindowsTextInput',
  };
});


require('@react-native-windows/tester/js/RNTesterApp');
