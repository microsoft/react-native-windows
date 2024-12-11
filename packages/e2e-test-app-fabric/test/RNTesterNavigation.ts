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
  const searchString = regexEscape(
    example.substring(0, Math.min(example.length, 8)),
  );

  // Filter the list down to the one test, to improve the stability of selectors
  const searchBox = await app.findElementByTestID('explorer_search');

  await app.waitUntil(
    async () => {
      await searchBox.setValue(searchString);
      return (await searchBox.getText()) === searchString;
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
  const exampleButton = await app.findElementByTestID(example);
  await exampleButton.waitForDisplayed({timeout: 5000});
  await exampleButton.click();

  const componentsTab = await app.findElementByTestID('example_search');
  await componentsTab.waitForDisplayed({timeout: 5000});
  expect(await componentsTab.isDisplayed()).toBe(true);
}

function regexEscape(str: string): string {
  return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}
