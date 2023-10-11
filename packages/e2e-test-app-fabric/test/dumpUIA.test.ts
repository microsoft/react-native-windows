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
  test('control', async () => {
    const componentsTabButton = await app.findElementByTestID('components-tab');
    await componentsTabButton.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('components-tab');
    expect(dump).toMatchSnapshot();
  });
});

export {};
