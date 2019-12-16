/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
import {
  REACT_CONTROL_ERROR_TEST_ID,
  HOME_BUTTON,
  TREE_DUMP_RESULT,
} from '../../app/Consts';

export function By(testId: string): WebdriverIO.Element {
  return $('~' + testId);
}

export function wait(timeout: number) {
  return new Promise(resolve => {
    setTimeout(resolve, timeout);
  });
}

export class BasePage {
  isPageLoaded(): boolean {
    return this.homeButton.isDisplayed();
  }

  waitForPageLoaded(timeout?: number) {
    browser.waitUntil(
      () => {
        return this.isPagedLoadedOrLoadBundleError();
      },
      this.timeoutForPageLoaded(timeout),
      'Wait for page ' + this.constructor.name + ' timeout'
    );
  }

  getTreeDumpResult() {
    var testResult = false;
    const maxWait = 20;
    var waitCount = 1;
    do {
      testResult = this.treeDumpResult.getText() == 'TreeDump:Passed';
      if (!testResult) {
        console.log(
          '####Waiting for treedump comparison result ' +
            waitCount +
            '/' +
            maxWait +
            '...####'
        );
        wait(100);
        waitCount += 1;
      }
    } while (waitCount <= maxWait && !testResult);

    return testResult;
  }

  protected timeoutForPageLoaded(currentTimeout?: number) {
    if (currentTimeout) return currentTimeout;
    return this.waitforPageTimeout;
  }

  protected get homeButton() {
    return By(HOME_BUTTON);
  }

  private get reactControlErrorMessage() {
    return By(REACT_CONTROL_ERROR_TEST_ID);
  }

  private isPagedLoadedOrLoadBundleError(): boolean {
    if (this.reactControlErrorMessage.isDisplayed()) {
      throw "ReactControl doesn't load bundle successfully: " +
        this.reactControlErrorMessage.getText();
    }
    return this.isPageLoaded();
  }

  private get treeDumpResult() {
    return By(TREE_DUMP_RESULT);
  }

  // Default timeout for waitForPageLoaded command in PageObject
  private waitforPageTimeout: number = 10000;
}
