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

const { toMatchImageSnapshot } = require('jest-image-snapshot');

expect.extend({ toMatchImageSnapshot });

const screenshotDir = './errorShots';
fs.mkdirSync(screenshotDir, {recursive: true});

process.env.NODE_ENV = 'test';

// Jest ENV does not have SampleTurboModule native module defined.
// This nightly no longer publishes Libraries/TurboModule/samples/NativeSampleTurboModule.
jest.mock(
  'react-native-windows/Libraries/TurboModule/samples/NativeSampleTurboModule',
  () => jest.fn(),
  {virtual: true},
);

// The tester copy calls TurboModuleRegistry.getEnforcing at import time.
// snapshotPages and visitAllPages only need the module graph to load.
jest.mock(
  '@react-native-windows/tester/js/examples/TurboModule/NativeSampleTurboModule',
  () => ({
    __esModule: true,
    EnumInt: {A: 23, B: 42},
    default: {},
  }),
);

// Jest ENV cannot resolve ..\..\relay.png since only relay@3.png exists
jest.mock(
  '@react-native-windows/tester/js/examples/AnimatedGratuitousApp/AnExChained',
  () => jest.fn(),
);

// disabled temporarily
//LogBox.ignoreAllLogs(true);
