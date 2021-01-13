/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { BasePage, by } from './BasePage';
import {
  APIS_NAV_BUTTON,
  COMPONENTS_NAV_BUTTON,
  TESTER_LIST_SEARCH_BOX,
} from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

class HomePage extends BasePage {
  async goToComponentExample(example: string) {
    await this.waitForElementLoaded(COMPONENTS_NAV_BUTTON);
    await (await by(COMPONENTS_NAV_BUTTON)).click();
    await this.goToExampleOnCurrentTab(example);
  }

  async goToApiExample(example: string) {
    await this.waitForElementLoaded(APIS_NAV_BUTTON);
    await (await by(APIS_NAV_BUTTON)).click();
    await this.goToExampleOnCurrentTab(example);
  }

  private async goToExampleOnCurrentTab(example: string) {
    // Filter the list down to the one test, to improve the stability of selectors
    await this.waitForElementLoaded(TESTER_LIST_SEARCH_BOX);
    const editBox = await by(TESTER_LIST_SEARCH_BOX);
    await editBox.setValue(example);
    const pageItem = await by(example);
    await pageItem.click();

    // Make sure we've launched the example by waiting until the search box is
    // no longer present, but make sure we haven't crashed by checking that nav
    // buttons are still visible
    await this.waitForElementHidden(TESTER_LIST_SEARCH_BOX);
    await this.waitForElementLoaded(COMPONENTS_NAV_BUTTON);
  }
}

export default new HomePage();
