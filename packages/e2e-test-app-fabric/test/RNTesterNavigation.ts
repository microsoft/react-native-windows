/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import type {AutomationElement} from '@react-native-windows/automation';

/**
 * Visit an example on the RNTester Components tab
 */
export async function goToComponentExample(example: string) {
  const componentsTabButton = await app.findElementByTestID('components-tab');
  await componentsTabButton.click();
  await goToExample(example);
}

/**
 * Visit an example on the RNTester APIs tab
 */
export async function goToApiExample(example: string) {
  const componentsTabButton = await app.findElementByTestID('apis-tab');
  await componentsTabButton.click();
  await goToExample(example);
}

async function goToExample(example: string) {
  const searchString = regexEscape(
    example.substring(0, Math.min(example.length, 8)),
  );

  // Filter the list down to the one test, to improve the stability of selectors
  const searchBox = await app.findElementByTestID('explorer_search');

  await app.waitUntil(
    async () => {
      await searchBox.setValue(searchString);
      const searchBoxText = await searchBox.getText();
      console.log(`Current searchBoxText: ${searchBoxText}`);
      return searchBoxText === searchString;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );

  // We cannot just click on exampleButton, since it it is likely off screen.
  // So we first search for the item hopfully causing the item to be one of the few remaining in the list - and therefore onscreen
  // Ideally we'd either use UIA to invoke the specific item, or ensure that the item is within view
  // Once we have those UIA patterns implemented we should update this logic.

  let exampleButton: AutomationElement;

  await app.waitUntil(
    async () => {
      exampleButton = await app.findElementByTestID(example);
      return true;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to find example button`,
    },
  );

  await app.waitUntil(
    async () => {
      await exampleButton.click();
      return true;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to click on example button`,
    },
  );

  // Make sure we've launched the example by waiting until the search box is
  // no longer present, but make sure we haven't crashed by checking that nav
  // buttons are still visible
  await app.waitUntil(async () => !(await exampleButton.isDisplayed()));
  const componentsTab = await app.findElementByTestID('components-tab');
  expect(await componentsTab.isDisplayed()).toBe(true);
}

function regexEscape(str: string): string {
  return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}
