/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, By } from '../pages/BasePage';

let pages = [
  'Alert',
  'Animated - Examples',
  'Animated - Gratuitous App',
  'Appearance',
  'AppState',
  'Border',
  'Box Shadow',
  'Clipboard',
  'Crash',
  'DevSettings',
  'Dimensions',
  'Keyboard',
  'Layout Events',
  'Linking',
  'Layout - Flexbox',
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
