/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('LegacySelectableTextTest');
});

describe('LegacySelectableTextTest', () => {
  beforeEach(async () => {
    await clearState();
  });

  test('PressableWhenNotSelectable', async () => {
    const textExample = await app.findElementByTestID('text-example');
    await textExample.click();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('DoubleClickWhenNotSelectable', async () => {
    const textExample = await app.findElementByTestID('text-example');
    await textExample.doubleClick();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('PressableWhenSelectable', async () => {
    await toggleSelectable();
    const textExample = await app.findElementByTestID('text-example');
    await textExample.click();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('DoubleClickWhenSelectable', async () => {
    await toggleSelectable();
    const textExample = await app.findElementByTestID('text-example');
    await textExample.doubleClick();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });
});

async function clearState() {
  const clearButton = await app.findElementByTestID('clear-state-button');
  await clearButton.click();
}

async function toggleSelectable() {
  const toggleButton = await app.findElementByTestID(
    'toggle-selectable-button',
  );
  await toggleButton.click();
}
