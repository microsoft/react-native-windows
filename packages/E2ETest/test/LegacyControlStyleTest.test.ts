/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {goToComponentExample, assertTreeDumpPassed} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('LegacyControlStyleTest');
});

describe('LegacyControlStyleTest', () => {
  /* Test case #1: Controls style with regular border */
  test('ControlStyleTestWithRegularBorder', async () => {
    await assertTreeDumpPassed();
  });

  /* Test case #2: Click button once, update controls style and round border*/
  test('ControlStyleTestWithRoundBorder', async () => {
    await toggleControlBorder();
    await assertTreeDumpPassed();
  });

  /* Test case #3: Click button one more, return to #1*/
  test('ControlStyleTestWithRegularBorder #2', async () => {
    await toggleControlBorder();
    await assertTreeDumpPassed();
  });
});

async function toggleControlBorder() {
  const showBorderToggle = await $('~show-border-toggle');
  await showBorderToggle.click();
}
