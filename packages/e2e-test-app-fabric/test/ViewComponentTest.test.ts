/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('View');
});

describe('ViewTests', () => {
  test('Views can have background colors', async () => {
    const dump = await dumpVisualTree('background-color');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have borders', async () => {
    const dump = await dumpVisualTree('border');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have padding and margins', async () => {
    const dump = await dumpVisualTree('padding-margin');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have rounded borders', async () => {
    const dump = await dumpVisualTree('rounded-borders');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have overflow', async () => {
    const dump = await dumpVisualTree('overflow');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have ranging opacity', async () => {
    const dump = await dumpVisualTree('opacity');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have tool tips', async () => {
    const dump = await dumpVisualTree('tool-tip');
    expect(dump).toMatchSnapshot();
  });
  test('Views can adjust backface visibility', async () => {
    const dump = await dumpVisualTree('backface-visibility');
    expect(dump).toMatchSnapshot();
  });
  test('Views can adjust backface visibility 2', async () => {
    const dump = await dumpVisualTree('backface-visibility-2');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have accessibility customization', async () => {
    const dump = await dumpVisualTree('accessibility');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have unique border width and radius per edge/corner', async () => {
    const dump = await dumpVisualTree('advanced-border');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a custom nativeID', async () => {
    const dump = await dumpVisualTree('nativeid');
    expect(dump).toMatchSnapshot();
  });
});
