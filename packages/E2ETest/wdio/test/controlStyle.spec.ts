/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import ControlStyleTestPage from '../pages/ControlStylePage';
import { CONTROL_STYLE_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(() => {
  HomePage.goToComponentExample(CONTROL_STYLE_TESTPAGE);
});

describe('ControlStyleTest', () => {
  /* Test case #1: Controls style with regular border */

  it('ControlStyleTestWithRegularBorder', () => {
    ControlStyleTestPage.waitForTreeDumpPassed(
      '#1. Dump comparison for Control style with regular border!'
    );
  });

  /* Test case #2: Click button once, update controls style and round border*/
  it('ControlStyleTestWithRoundBorder', () => {
    ControlStyleTestPage.toggleControlBorder();
    ControlStyleTestPage.waitForTreeDumpPassed(
      '#2. Dump comparison for Control style with round border!'
    );
  });

  /* Test case #3: Click button one more, return to #1*/
  it('ControlStyleTestWithRegularBorder #2', () => {
    ControlStyleTestPage.toggleControlBorder();
    ControlStyleTestPage.waitForTreeDumpPassed(
      '#3. Second dump comparison for Control style with regular border!'
    );
  });
});
