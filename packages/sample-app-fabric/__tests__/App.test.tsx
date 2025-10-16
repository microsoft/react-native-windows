/**
 * @format
 */

// Add this import to fix the mockComponent reference error
import mockComponent from 'react-native-windows/jest/mockComponent';
global.mockComponent = mockComponent;

import 'react-native';
import React from 'react';
import App from '../App';

// Note: import explicitly to use the types shipped with jest.
import {it} from '@jest/globals';

// Note: test renderer must be required after react-native.
import renderer from 'react-test-renderer';

it('renders correctly', () => {
  renderer.create(<App />);
});
