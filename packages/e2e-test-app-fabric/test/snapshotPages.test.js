const React = require('react');
import {create, act} from 'react-test-renderer';

test('Control', () => {
  const tree = create('View').toJSON();
  expect(tree).toMatchSnapshot();
});

test('Control2', () => {
  global.__fbBatchedBridgeConfig = true;
  const PlatformConstants = 'react-native/Libraries/Utilities/NativePlatformConstantsAndroid.js';

  jest.mock('react-native/Libraries/Utilities/NativePlatformConstantsAndroid.js');

  const Example = require('@react-native-windows/tester/js/examples-win/Switch/SwitchExample.windows');
  const tree = create(<Example/>).toJSON();
  expect(tree).toMatchSnapshot();
});