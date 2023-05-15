const React = require('react');
import {create, act} from 'react-test-renderer';
const Example = require('@react-native-windows/tester/js/examples/View/ViewExample.windows');

test('Control', () => {
  const tree = create('View').toJSON();
  expect(tree).toMatchSnapshot();
});

test('Control2', () => {
  const tree = create(<Example/>).toJSON();
  expect(tree).toMatchSnapshot();
});