/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';

/**
 * Visit an example on the RNTester Components tab
 */
export async function goToComponentExample(example: string) {
  const componentsTabButton = await app.findElementByTestID('components-tab');
  await componentsTabButton.waitForDisplayed({timeout: 20000});
  await componentsTabButton.click();
  await goToExample(example);
}

/**
 * Visit an example on the RNTester APIs tab
 */
export async function goToApiExample(example: string) {
  const componentsTabButton = await app.findElementByTestID('apis-tab');
  await componentsTabButton.waitForDisplayed({timeout: 20000});
  await componentsTabButton.click();
  await goToExample(example);
}

async function goToExample(example: string) {
  // Filter the list down to the one test, to improve the stability of selectors
  const searchBox = await app.findElementByTestID('explorer_search');
  await searchBox.addValue(['Backspace', 'Backspace', 'Backspace']);
  // Only grab first three characters of string to reduce cases in WebDriverIO mistyping.
  await searchBox.addValue(regexEscape(example.substring(0, 3)));
  const exampleButton = await app.findElementByTestID(example);
  await exampleButton.waitForDisplayed({timeout: 5000});
  await exampleButton.click();

  const componentsTab = await app.findElementByTestID('components-tab');
  await componentsTab.waitForDisplayed({timeout: 5000});
  expect(await componentsTab.isDisplayed()).toBe(true);
}

function regexEscape(str: string): string {
  return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}
