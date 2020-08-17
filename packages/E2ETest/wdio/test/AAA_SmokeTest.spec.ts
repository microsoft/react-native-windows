/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
import { By } from '../pages/BasePage';
import { CONTROL_STYLE_TESTPAGE } from 'react-native-windows/RNTester/js/examples-win/LegacyTests/Consts';

describe('SmokeTest', () => {
  it('SmokeTest', () => {
    // Only click the button to try to go to the ControlStyle test page,
    // to workaround an instability in WinAppDriver (?) on first test launch,
    // which causes the click to not actually work if the control needs to
    // first be scrolled into view.
    By(CONTROL_STYLE_TESTPAGE).click();
  });
});
