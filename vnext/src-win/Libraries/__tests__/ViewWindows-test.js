'use strict';

// Add these imports to fix the reference errors
import mockComponent from 'react-native/jest/mockComponent';
import MockNativeMethods from 'react-native/jest/MockNativeMethods';
global.mockComponent = mockComponent;
global.MockNativeMethods = MockNativeMethods;

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