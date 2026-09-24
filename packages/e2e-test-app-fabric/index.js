require('react-native');

const {IMAGE_FIXTURE_BASE_URL} = require('./imageFixtureConfig');

// The RNTester Image examples download their remote assets from GitHub. Point
// this app at the loopback fixture server started by jest (globalSetup.js) so
// the Image tests are hermetic. RNTester itself keeps the remote assets.
global.__RNW_E2E_IMAGE_BASE_URL__ = IMAGE_FIXTURE_BASE_URL;

function componentHasNativeconfig(name: string) {
    return !global.RN$Bridgeless && name !== 'CustomXamlComponentWithYogaLayout' && name !== 'CustomXamlComponentWithNativeLayout'
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