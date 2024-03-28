/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('ActivityIndicator');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('ActivityIndicator Tests', () => {
  test('An ActivityIndicator can render', async () => {
    const component = await app.findElementByTestID(
      'default_activity_indicator',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('default_activity_indicator');
    expect(dump).toMatchSnapshot();
  });
  test('An ActivityIndicator can have styling', async () => {
    const component = await app.findElementByTestID('activity-gray');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('activity-gray');
    expect(dump).toMatchSnapshot();
  });
  test('An ActivityIndicator can have custom colors', async () => {
    const component = await app.findElementByTestID('activity-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('activity-color');
    expect(dump).toMatchSnapshot();
  });
  test('An ActivityIndicator can have large sizing', async () => {
    const component = await app.findElementByTestID('activity-large');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('activity-large');
    expect(dump).toMatchSnapshot();
  });
  test('An ActivityIndicator can have custom colors and large sizing', async () => {
    const component = await app.findElementByTestID('activity-large-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('activity-large-color');
    expect(dump).toMatchSnapshot();
  });
  test('An ActivityIndicator can have toggle animation', async () => {
    const component = await app.findElementByTestID('activity-toggle');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('activity-toggle');
    expect(dump).toMatchSnapshot();
  });
  test('An ActivityIndicator can have custom sizing', async () => {
    const component = await app.findElementByTestID('activity-size');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('activity-size');
    expect(dump).toMatchSnapshot();
  });
});
