/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const path = require('path');
const sanitizeFilename = require('sanitize-filename');
// disabled temporarily
//const {LogBox} = require('react-native');

const screenshotDir = './errorShots';
fs.mkdirSync(screenshotDir, {recursive: true});

process.env.NODE_ENV = 'test';

// Jest ENV does not have SampleTurboModule native module defined
jest.mock(
  'react-native-windows/Libraries/TurboModule/samples/NativeSampleTurboModule',
  () => jest.fn(),
);

// Jest ENV cannot resolve ..\..\relay.png since only relay@3.png exists
jest.mock(
  '@react-native-windows/tester/js/examples/AnimatedGratuitousApp/AnExChained',
  () => jest.fn(),
);

// disabled temporarily
//LogBox.ignoreAllLogs(true);
