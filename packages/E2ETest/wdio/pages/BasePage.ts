/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { TREE_DUMP_RESULT } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

const ELEMENT_LOADED_TIMEOUT = 60000;

export function by(testId: string): WebdriverIO.Element {
  return $('~' + testId);
}

export class BasePage {
  isElementLoaded(element: string): boolean {
    return by(element).isDisplayed();
  }

  waitForElementLoaded(element: string) {
    browser.waitUntil(() => this.isElementLoaded(element), {
      timeout: ELEMENT_LOADED_TIMEOUT,
      timeoutMsg: `Failed to find element with testId "${element}" within ${ELEMENT_LOADED_TIMEOUT}ms`,
    });
  }

  waitForElementHidden(element: string) {
    browser.waitUntil(() => !this.isElementLoaded(element), {
      timeout: ELEMENT_LOADED_TIMEOUT,
      timeoutMsg: `Element with testId "${element}" was not hidden within ${ELEMENT_LOADED_TIMEOUT}ms`,
    });
  }

  waitForTreeDumpPassed(errorMessage: string) {
    browser.waitUntil(
      () => this.treeDumpResult.getText() === 'TreeDump:Passed',
      {
        timeout: ELEMENT_LOADED_TIMEOUT,
        timeoutMsg: errorMessage,
      }
    );
  }

  private get treeDumpResult() {
    return by(TREE_DUMP_RESULT);
  }
}
