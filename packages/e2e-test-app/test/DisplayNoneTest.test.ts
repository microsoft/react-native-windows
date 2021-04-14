/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, dumpVisualTree} from './framework';

beforeAll(async () => {
  await goToComponentExample('Display:none Style');
});

describe('DisplayNoneTest', () => {
  /* Test case #1: display:none disabled */
  test('DisplayNoneDisabledTest', async () => {
    const dump = await dumpVisualTree('view-component-switch-view');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Enable display:none once, update view*/
  test('DisplayNoneEnabledTest', async () => {
    await toggleDisplayNone();
    const dump = await dumpVisualTree('view-component-switch-view');
    expect(dump).toMatchSnapshot();
  });
});

async function toggleDisplayNone() {
  const showDisplayNoneToggle = await $('~toggle-display:none');
  await showDisplayNoneToggle.click();
}
