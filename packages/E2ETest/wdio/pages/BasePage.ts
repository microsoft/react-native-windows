/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { TREE_DUMP_RESULT } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

const ELEMENT_LOADED_TIMEOUT = 60000;

export function By(testId: string): WebdriverIO.Element {
  return $('~' + testId);
}

export class BasePage {
  isElementLoaded(element: string): boolean {
    return By(element).isDisplayed();
  }

  waitForElementLoaded(element: string) {
    browser.waitUntil(
      () => this.isElementLoaded(element),
      ELEMENT_LOADED_TIMEOUT,
      `Failed to find element with testId "${element}" within ${ELEMENT_LOADED_TIMEOUT}ms`
    );
  }

  waitForElementHidden(element: string) {
    browser.waitUntil(
      () => !this.isElementLoaded(element),
      ELEMENT_LOADED_TIMEOUT,
      `Element with testId "${element}" was not hidden within ${ELEMENT_LOADED_TIMEOUT}ms`
    );
  }

  waitForTreeDumpPassed(errorMessage: string) {
    browser.waitUntil(
      () => this.treeDumpResult.getText() === 'TreeDump:Passed',
      ELEMENT_LOADED_TIMEOUT,
      errorMessage
    );
  }

  private get treeDumpResult() {
    return By(TREE_DUMP_RESULT);
  }
}
