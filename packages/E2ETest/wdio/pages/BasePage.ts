/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { TREE_DUMP_RESULT } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

const ELEMENT_LOADED_TIMEOUT = 60000;

export function by(testId: string): Promise<WebdriverIOAsync.Element> {
  return $('~' + testId);
}

export class BasePage {
  async isElementLoaded(element: string): Promise<boolean> {
    return (await by(element)).isDisplayed();
  }

  waitForElementLoaded(element: string) {
    return browser.waitUntil(() => this.isElementLoaded(element), ELEMENT_LOADED_TIMEOUT,
      `Failed to find element with testId "${element}" within ${ELEMENT_LOADED_TIMEOUT}ms`,
      );
  }

  waitForElementHidden(element: string) {
    return browser.waitUntil(async () => !this.isElementLoaded(element), ELEMENT_LOADED_TIMEOUT,
      `Element with testId "${element}" was not hidden within ${ELEMENT_LOADED_TIMEOUT}ms`,
    );
  }

  waitForTreeDumpPassed(errorMessage: string) {
    return browser.waitUntil(
      async () => (await (await this.treeDumpResult).getText()) === 'TreeDump:Passed',
      ELEMENT_LOADED_TIMEOUT,
      errorMessage,
    );
  }

  private get treeDumpResult() {
    return by(TREE_DUMP_RESULT);
  }
}
