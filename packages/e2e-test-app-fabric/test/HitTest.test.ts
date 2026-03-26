/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Hit Testing');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

async function verifyElementAccessibiltyValue(element: string, value: string) {
  const dump = await dumpVisualTree(element);
  expect(dump!['Automation Tree']['ValuePattern.Value']).toBe(value);
}

describe('Hit Testing', () => {
  test('Hit testing child outside the bounds of parents', async () => {
    const target = await app.findElementByTestID('visible-overflow-element');

    // View starts in red state
    await verifyElementAccessibiltyValue('visible-overflow-element', 'red');

    // The webdriverio package computes the offsets from the center point of the target.
    // This is within the bounds of the child and the parent, so should hitTest even with overflow:visible
    await target.click({x: -50, y: -50});

    await verifyElementAccessibiltyValue('visible-overflow-element', 'green');

    // The webdriverio package computes the offsets from the center point of the target.
    // This is within the bounds of the child, but outside the parents bounds
    await target.click({x: 0, y: 0});

    // View should still be red, since the click should hit the pressable
    await verifyElementAccessibiltyValue('visible-overflow-element', 'red');
  });

  test('Overflow hidden prevents hittesting child', async () => {
    const target = await app.findElementByTestID('hidden-overflow-element');

    // View starts in red state
    await verifyElementAccessibiltyValue('hidden-overflow-element', 'red');

    // This is within the bounds of the child and the parent, so should hitTest even with overflow:hidden
    await target.click({x: -50, y: -50});

    await verifyElementAccessibiltyValue('hidden-overflow-element', 'green');

    // This is within the bounds of the child, but shouldn't hit test, since the parent is overflow:hidden
    await target.click({x: 0, y: 0});

    // View should still be green, since the click shouldn't hit the pressable
    await verifyElementAccessibiltyValue('hidden-overflow-element', 'green');
  });
});
