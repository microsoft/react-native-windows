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
  await goToComponentExample('View');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('View Tests', () => {
  test('Control', () => {
    expect(true).toBe(true);
  });
  test('Views can have shadows', async () => {
    const component = await app.findElementByTestID('shadow');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('shadow');
    expect(dump).toMatchSnapshot();
  });
  // Disable until tests pass in CI.
  /*test('Views can have border styles', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await searchBox.addValue(['Backspace', 'Backspace', 'Backspace']);
    await searchBox.addValue('sty');
    const componentsTab = await app.findElementByTestID('border-style-button');
    await componentsTab.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('border-style-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have offscreen alpha compositing', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await searchBox.addValue(['Backspace', 'Backspace', 'Backspace']);
    await searchBox.addValue('off');
    const componentsTab = await app.findElementByTestID('offscreen-alpha-compositing-button');
    await componentsTab.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('offscreen-alpha-compositing-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a z-index', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await searchBox.addValue(['Backspace', 'Backspace', 'Backspace']);
    await searchBox.addValue('z');
    const componentsTab = await app.findElementByTestID('z-index-button');
    await componentsTab.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('z-index-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have display: none', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await searchBox.addValue(['Backspace', 'Backspace', 'Backspace']);
    await searchBox.addValue('dis');
    const componentsTab = await app.findElementByTestID('display-none-button');
    await componentsTab.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('display-none-button');
    expect(dump).toMatchSnapshot();
  });
  */
});
