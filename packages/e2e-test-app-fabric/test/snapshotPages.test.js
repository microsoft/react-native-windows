const React = require('react');
import {create, act} from 'react-test-renderer';
import {View} from 'react-native';

test('Component Control', () => {
  const tree = create(<View />).toJSON();
  expect(tree).toMatchSnapshot();
});