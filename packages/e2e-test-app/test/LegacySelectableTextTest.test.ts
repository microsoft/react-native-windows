/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, dumpVisualTree} from './framework';

beforeAll(async () => {
  await goToComponentExample('LegacySelectableTextTest');
});

describe('LegacySelectableTextTest', () => {
  beforeEach(async () => {
    await clearState();
  });

  test('PressableWhenNotSelectable', async () => {
    const textExample = await $('~text-example');
    await textExample.click();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('DoubleClickWhenNotSelectable', async () => {
    const textExample = await $('~text-example');
    await textExample.doubleClick();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('PressableWhenSelectable', async () => {
    await toggleSelectable();
    const textExample = await $('~text-example');
    await textExample.click();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('DoubleClickWhenSelectable', async () => {
    await toggleSelectable();
    const textExample = await $('~text-example');
    await textExample.doubleClick();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });
});

async function clearState() {
  const clearButton = await $('~clear-state-button');
  await clearButton.click();
}

async function toggleSelectable() {
  const toggleButton = await $('~toggle-selectable-button');
  await toggleButton.click();
}
