/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, By } from '../pages/BasePage';

let pages = [
  'Native Animated Example',
  'PanResponder Sample',
  'PlatformColor',
  'Pointer Events',
  'RTLExample',
  'Share',
  'Timers',
  'AppTheme',
  'WebSocket',
  'Transforms',
];

class TestPage extends BasePage {
  goToTestPage(page: string) {
    // Filter the list down to the one test, to improve stability of selectors
    this.waitForHomePageLoaded();
    let editBox = By('explorer_search');
    editBox.setValue(page);
    let pageItem = By(page);
    pageItem.click();
    this.waitForPageLoaded();
  }
  backToHomePage() {
    this.homeButton.click();
  }

  waitForHomePageLoaded(timeout?: number) {
    // eslint-disable-next-line no-undef
    browser.waitUntil(
      () => {
        return By('explorer_search').isDisplayed();
      },
      this.timeoutForPageLoaded(timeout),
      'Wait for HomePage load timed out'
    );
  }
}

describe('VisitAllPagesTest', () => {
  pages.forEach(function(page) {
    it(page, () => {
      console.log('loading page ' + page);
      let testPage = new TestPage();
      testPage.goToTestPage(page);
      testPage.backToHomePage();
    });
  });
});
