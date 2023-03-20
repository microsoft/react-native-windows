
// This turns on validation that the ViewManager viewconfigs align with the JS ViewConfigs.
const nativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
nativeComponentRegistry.setRuntimeConfigProvider((_name: string) => {
  return {
    native: true,
    strict: false,
    verify: true,
  };
});


require('@react-native-windows/tester/js/RNTesterApp');
