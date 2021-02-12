/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, dumpVisualTree} from './framework';

beforeAll(async () => {
  await goToComponentExample('LegacyControlStyleTest');
});

describe('LegacyControlStyleTest', () => {
  /* Test case #1: Controls style with regular border */
  test('ControlStyleTestWithRegularBorder', async () => {
    const dump = await dumpVisualTree('control-style-switch-view');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Click button once, update controls style and round border*/
  test('ControlStyleTestWithRoundBorder', async () => {
    await toggleControlBorder();
    const dump = await dumpVisualTree('control-style-switch-view');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #3: Click button one more, return to #1*/
  test('ControlStyleTestWithRegularBorder #2', async () => {
    await toggleControlBorder();
    const dump = await dumpVisualTree('control-style-switch-view');
    expect(dump).toMatchSnapshot();
  });
});

async function toggleControlBorder() {
  const showBorderToggle = await $('~show-border-toggle');
  await showBorderToggle.click();
}
