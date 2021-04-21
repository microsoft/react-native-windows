/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, dumpVisualTree} from './framework';

beforeAll(async () => {
  await goToComponentExample('LegacyImageTest');
});

describe('LegacyImageTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  test('ImageWithoutBorderTest', async () => {
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Click button once, update view and image with round border*/
  test('ImageWithBorderTest', async () => {
    await toggleImageBorder();
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #3: Click button one more, remove border from view and image but tree sturcture is different from #1*/
  test('ImageWithoutBorderTestOneMoreClick', async () => {
    await toggleImageBorder();
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });

  test('ImageRTLTest', async () => {
    await toggleRTLMode();
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
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
