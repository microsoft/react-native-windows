/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, app} from './framework';

beforeAll(async () => {
  await goToComponentExample('Display:none Style');
});

describe('DisplayNoneTest', () => {
  /* Test case #1: display:none disabled */
  test('DisplayNoneDisabledTest', async () => {
    const dump = await app.dumpVisualTree('textbox-container');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Enable display:none once, update view*/
  test('DisplayNoneEnabledTest', async () => {
    await toggleDisplayNone();
    const dump = await app.dumpVisualTree('textbox-container');
    expect(dump).toMatchSnapshot();
  });
});

async function toggleDisplayNone() {
  const showDisplayNoneToggle = await app.findElementByTestID(
    'toggle-display:none',
  );
  await showDisplayNoneToggle.click();
}
