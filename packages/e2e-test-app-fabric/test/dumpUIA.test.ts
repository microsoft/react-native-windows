/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';
import {dumpVisualTree} from '@react-native-windows/automation-commands';

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('dumpUIA', () => {
  test('Button UIA Dump', async () => {
    const componentsTabButton = await app.findElementByTestID('components-tab');
    await componentsTabButton.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('components-tab');
    expect(dump).toMatchSnapshot();
  });
  test('Search Bar UIA Dump', async () => {
    const dump = await dumpVisualTree('apis-tab');
    expect(dump).toMatchSnapshot();
  });
  test('Search Bar UIA Dump', async () => {
    const dump = await dumpVisualTree('explorer_search');
    expect(dump).toMatchSnapshot();
  });
});

export {};
