/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { goToComponentExample, assertTreeDumpPassed } from './Helpers';

beforeAll(async () => {
  await goToComponentExample('LegacyImageTest');
});

describe('ImageWithoutBorderTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  it('ImageWithoutBorderTest', async () => {
    await assertTreeDumpPassed();
  });

  /* Test case #2: Click button once, update view and image with round border*/
  it('ImageWithBorderTest', async () => {
    await toggleImageBorder();
    await assertTreeDumpPassed();
  });

  /* Test case #3: Click button one more, remove border from view and image but tree sturcture is different from #1*/
  it('ImageWithoutBorderTestOneMoreClick', async () => {
    await toggleImageBorder();
    await assertTreeDumpPassed();
  });

  it('ImageRTLTest', async () => {
    await toggleRTLMode();
    await assertTreeDumpPassed();
  });
});

async function toggleImageBorder() {
  const imageBorderToggle = await $('~toggle-border-button');
  await imageBorderToggle.click();
}

async function toggleRTLMode() {
  const rtlToggleButton = await $('~set-rtl-button');
  await rtlToggleButton.click();
}
