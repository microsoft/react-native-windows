/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import ControlStyleTestPage from '../pages/ControlStylePage';
import { CONTROL_STYLE_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(async () => {
  await HomePage.goToComponentExample(CONTROL_STYLE_TESTPAGE);
});

describe('ControlStyleTest', async () => {
  /* Test case #1: Controls style with regular border */

  it('ControlStyleTestWithRegularBorder', async () => {
    await ControlStyleTestPage.waitForTreeDumpPassed(
      '#1. Dump comparison for Control style with regular border!'
    );
  });

  /* Test case #2: Click button once, update controls style and round border*/
  it('ControlStyleTestWithRoundBorder', async () => {
    await ControlStyleTestPage.toggleControlBorder();
    await ControlStyleTestPage.waitForTreeDumpPassed(
      '#2. Dump comparison for Control style with round border!'
    );
  });

  /* Test case #3: Click button one more, return to #1*/
  it('ControlStyleTestWithRegularBorder #2', async () => {
    await ControlStyleTestPage.toggleControlBorder();
    await ControlStyleTestPage.waitForTreeDumpPassed(
      '#3. Second dump comparison for Control style with regular border!'
    );
  });
});
