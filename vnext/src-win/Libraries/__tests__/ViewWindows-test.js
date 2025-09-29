
'use strict';

// Add this import to fix the mockComponent reference error
import mockComponent from 'react-native/jest/mockComponent';
global.mockComponent = mockComponent;

const React = require('react');
const renderer = require('react-native/jest/renderer');
const View = require('../Components/View/View').default;

describe('View', () => {
  it('renders basic view', async () => {
    expect(
      await renderer.create(
      <View/>
    ),
  ).toMatchSnapshot();
  });
});