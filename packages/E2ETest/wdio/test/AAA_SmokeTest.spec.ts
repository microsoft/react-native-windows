/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
import HomePage from '../pages/HomePage';

describe('SmokeTest', () => {
  it('SmokeTest', () => {
    // Only click the button to go to the ControlStyle test page,
    // to workaround an instability in WinAppDriver (?) on first test launch,
    // which causes the click to not actually work if the control needs to
    // first be scrolled into view.
    HomePage.clickControlStylePageButton();
  });
});
