const React = require('react');
import {create, act} from 'react-test-renderer';

test('Control', () => {
  const tree = create('View').toJSON();
  expect(tree).toMatchSnapshot();
});

test('Control2', () => {
  const Example = require('@react-native-windows/tester/js/examples-win/Switch/SwitchExample');
  const tree = create(<Example/>).toJSON();
  expect(tree).toMatchSnapshot();
});