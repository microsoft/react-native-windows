/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import {
  APIS_NAV_BUTTON,
  COMPONENTS_NAV_BUTTON,
  TESTER_LIST_SEARCH_BOX,
} from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

class HomePage extends BasePage {
  goToComponentExample(example: string) {
    this.waitForElementLoaded(COMPONENTS_NAV_BUTTON);
    By(COMPONENTS_NAV_BUTTON).click();
    this.goToExampleOnCurrentTab(example);
  }

  goToApiExample(example: string) {
    this.waitForElementLoaded(APIS_NAV_BUTTON);
    By(APIS_NAV_BUTTON).click();
    this.goToExampleOnCurrentTab(example);
  }

  private goToExampleOnCurrentTab(example: string) {
    // Filter the list down to the one test, to improve the stability of selectors
    this.waitForElementLoaded(TESTER_LIST_SEARCH_BOX);
    const editBox = By(TESTER_LIST_SEARCH_BOX);
    editBox.setValue(example);
    const pageItem = By(example);
    pageItem.click();

    // Make sure we've launched the example by waiting until the search box is
    // no longer present, but make sure we haven't crashed by checking that nav
    // buttons are still visible
    this.waitForElementHidden(TESTER_LIST_SEARCH_BOX);
    this.waitForElementLoaded(COMPONENTS_NAV_BUTTON);
  }
}

export default new HomePage();
