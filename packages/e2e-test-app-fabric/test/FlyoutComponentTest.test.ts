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
  await goToComponentExample('Flyout');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Flyout Tests', () => {
  test('A Flyout container is displayed with default state', async () => {
    const component = await app.findElementByTestID('flyout');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flyout');
    expect(dump).toMatchSnapshot();
  });

  test('A Flyout can be opened with a target', async () => {
    // Click the "Open Flyout" button to show the flyout
    const openButton = await app.findElementByXPath(
      '//Button[@Name="Open Flyout"]',
    );
    await openButton.waitForDisplayed({timeout: 5000});
    await openButton.click();

    // Wait for flyout to appear and capture its state
    const flyout = await app.findElementByTestID('flyout-accessibility');
    await flyout.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flyout-accessibility');
    expect(dump).toMatchSnapshot();

    // Close the flyout
    const closeButton = await app.findElementByXPath('//Button[@Name="Close"]');
    await closeButton.click();
  });

  test('A Flyout can be opened without a target', async () => {
    // Click the "Open Flyout without Target" button
    const openButton = await app.findElementByXPath(
      '//Button[@Name="Open Flyout without Target"]',
    );
    await openButton.waitForDisplayed({timeout: 5000});
    await openButton.click();

    // Wait for flyout to appear and capture its state
    const flyout = await app.findElementByTestID('flyout-accessibility-3');
    await flyout.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flyout-accessibility-3');
    expect(dump).toMatchSnapshot();

    // Close the flyout by clicking outside (light dismiss)
    const mainContainer = await app.findElementByTestID('flyout');
    await mainContainer.click();
  });

  test('A Flyout can be opened with offset', async () => {
    // Click the "Open Flyout with Offset" button
    const openButton = await app.findElementByXPath(
      '//Button[@Name="Open Flyout with Offset"]',
    );
    await openButton.waitForDisplayed({timeout: 5000});
    await openButton.click();

    // Wait for flyout to appear and capture its state
    const flyout = await app.findElementByTestID('flyout-accessibility-4');
    await flyout.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flyout-accessibility-4');
    expect(dump).toMatchSnapshot();

    // Close the flyout by clicking outside (light dismiss)
    const mainContainer = await app.findElementByTestID('flyout');
    await mainContainer.click();
  });

  test('A Flyout can have different placement options', async () => {
    // Test different placement options by changing the picker value
    const placementPicker = await app.findElementByTestID('placement-picker');
    if (await placementPicker.isDisplayed()) {
      // Change to bottom placement
      await placementPicker.selectByVisibleText('bottom');

      // Open flyout to test bottom placement
      const openButton = await app.findElementByXPath(
        '//Button[@Name="Open Flyout"]',
      );
      await openButton.click();

      const flyout = await app.findElementByTestID('flyout-accessibility');
      await flyout.waitForDisplayed({timeout: 5000});
      const dump = await dumpVisualTree('flyout-accessibility');
      expect(dump).toMatchSnapshot();

      // Close the flyout
      const closeButton = await app.findElementByXPath(
        '//Button[@Name="Close"]',
      );
      await closeButton.click();
    }
  });

  test('A Flyout can be nested with another Flyout', async () => {
    // Open the main flyout first
    const openButton = await app.findElementByXPath(
      '//Button[@Name="Open Flyout"]',
    );
    await openButton.waitForDisplayed({timeout: 5000});
    await openButton.click();

    // Wait for main flyout to appear
    const flyout = await app.findElementByTestID('flyout-accessibility');
    await flyout.waitForDisplayed({timeout: 5000});

    // Open another flyout from within the first one
    const openAnotherButton = await app.findElementByXPath(
      '//Button[@Name="Open Another Flyout"]',
    );
    await openAnotherButton.waitForDisplayed({timeout: 5000});
    await openAnotherButton.click();

    // Wait for second flyout to appear and capture state
    const secondFlyout = await app.findElementByTestID(
      'flyout-accessibility-2',
    );
    await secondFlyout.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('flyout-accessibility-2');
    expect(dump).toMatchSnapshot();

    // Close both flyouts by clicking outside
    const mainContainer = await app.findElementByTestID('flyout');
    await mainContainer.click();
  });

  test('A Flyout can contain a Popup', async () => {
    // Open the main flyout first
    const openButton = await app.findElementByXPath(
      '//Button[@Name="Open Flyout"]',
    );
    await openButton.waitForDisplayed({timeout: 5000});
    await openButton.click();

    // Wait for main flyout to appear
    const flyout = await app.findElementByTestID('flyout-accessibility');
    await flyout.waitForDisplayed({timeout: 5000});

    // Open a popup from within the flyout
    const openPopupButton = await app.findElementByXPath(
      '//Button[@Name="Open A Popup"]',
    );
    await openPopupButton.waitForDisplayed({timeout: 5000});
    await openPopupButton.click();

    // Wait for popup to appear and capture state
    // Note: Popup testID might need to be determined from the actual implementation
    const dump = await dumpVisualTree('flyout-accessibility');
    expect(dump).toMatchSnapshot();

    // Close the popup and flyout
    const mainContainer = await app.findElementByTestID('flyout');
    await mainContainer.click();
  });
});
