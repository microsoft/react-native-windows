/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import {
  SEARCH_BOX,
  BACK_BUTTON,
} from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

class HomePage extends BasePage {
  goToTestPage(page: string) {
    // Filter the list down to the one test, to improve the stability of selectors
    this.waitForElementLoaded(SEARCH_BOX);
    let editBox = By(SEARCH_BOX);
    editBox.setValue(page);
    let pageItem = By(page);
    pageItem.click();
    super.waitForElementLoaded(BACK_BUTTON);
  }

  backToHomePage() {
    this.homeButton.click();
  }
}

export default new HomePage();
