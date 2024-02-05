/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('View');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('ViewTests', () => {
  test('Views can have background colors', async () => {
    const dump = await dumpVisualTree('view-test-background-color');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have borders', async () => {
    const dump = await dumpVisualTree('view-test-border');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have padding and margins', async () => {
    const dump = await dumpVisualTree('view-test-padding-margin');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have rounded borders', async () => {
    const dump = await dumpVisualTree('view-test-rounded-borders');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have overflow', async () => {
    const dump = await dumpVisualTree('view-test-overflow');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have ranging opacity', async () => {
    const dump = await dumpVisualTree('view-test-opacity');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have tool tips', async () => {
    const dump = await dumpVisualTree('tool-tip');
    expect(dump).toMatchSnapshot();
  });
  test('Views can adjust backface visibility', async () => {
    const dump = await dumpVisualTree('view-test-backface-visibility');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have accessibility customization', async () => {
    const dump = await dumpVisualTree('accessibility');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a custom nativeID', async () => {
    const dump = await dumpVisualTree('nativeid');
    expect(dump).toMatchSnapshot();
  });
});
