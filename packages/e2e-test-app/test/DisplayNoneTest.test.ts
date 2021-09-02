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
  await goToComponentExample('Display:none Style');
});

describe('DisplayNoneTest', () => {
  /* Test case #1: display:none disabled */
  test('DisplayNoneDisabledTest', async () => {
    const dump = await dumpVisualTree('textbox-container');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Enable display:none once, update view*/
  test('DisplayNoneEnabledTest', async () => {
    await toggleDisplayNone();
    const dump = await dumpVisualTree('textbox-container');
    expect(dump).toMatchSnapshot();
  });
});

async function toggleDisplayNone() {
  const showDisplayNoneToggle = await app.findElementByTestID(
    'toggle-display:none',
  );
  await showDisplayNoneToggle.click();
}
