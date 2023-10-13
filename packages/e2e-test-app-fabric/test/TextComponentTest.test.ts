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
  await goToComponentExample('Text');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Text Tests', () => {
  test('Text can wrap', async () => {
    const dump = await dumpVisualTree('text-wrap');
    expect(dump).toMatchSnapshot();
  });
  test('Padding can be added to Text', async () => {
    const dump = await dumpVisualTree('text-padding');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have a font family', async () => {
    const dump = await dumpVisualTree('text-fontFamily');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have a size', async () => {
    const dump = await dumpVisualTree('text-size');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have a color', async () => {
    const dump = await dumpVisualTree('text-color');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have decoration lines: Underline', async () => {
    const dump = await dumpVisualTree('text-decoration-underline');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have decoration lines: Solid Line Through', async () => {
    const dump = await dumpVisualTree('text-decoration-solid-linethru');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have an outer color', async () => {
    const dump = await dumpVisualTree('text-outer-color');
    expect(dump).toMatchSnapshot();
  });
  test('Text can be restricted to one line', async () => {
    const dump = await dumpVisualTree('text-one-line');
    expect(dump).toMatchSnapshot();
  });
  test('Text can be selectable', async () => {
    const dump = await dumpVisualTree('text-selectable');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have a customized selection color', async () => {
    const dump = await dumpVisualTree('text-selection-color');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have shadows', async () => {
    const dump = await dumpVisualTree('text-shadow');
    expect(dump).toMatchSnapshot();
  });
});
