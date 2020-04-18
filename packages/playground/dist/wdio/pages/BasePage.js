"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
// import {
//   REACT_CONTROL_ERROR_TEST_ID,
//   HOME_BUTTON,
//   TREE_DUMP_RESULT,
// } from '../../app/Consts';
Object.defineProperty(exports, "__esModule", { value: true });
const REACT_CONTROL_ERROR_TEST_ID = 'ReactControlErrorMessage';
function By(testId) {
    return $('~' + testId);
}
exports.By = By;
function wait(timeout) {
    return new Promise(resolve => {
        setTimeout(resolve, timeout);
    });
}
exports.wait = wait;
class BasePage {
    constructor() {
        // private get treeDumpResult() {
        //   return By(TREE_DUMP_RESULT);
        // }
        // Default timeout for waitForPageLoaded command in PageObject
        this.waitforPageTimeout = 10000;
    }
    isPageLoaded() {
        return this.loadButton.isDisplayed();
    }
    waitForPageLoaded(timeout) {
        browser.waitUntil(() => {
            return this.isPagedLoadedOrLoadBundleError();
        }, this.timeoutForPageLoaded(timeout), 'Wait for page ' + this.constructor.name + ' timeout');
    }
    // getTreeDumpResult() {
    //   var testResult = false;
    //   const maxWait = 20;
    //   var waitCount = 1;
    //   do {
    //     testResult = this.treeDumpResult.getText() == 'TreeDump:Passed';
    //     if (!testResult) {
    //       console.log(
    //         '####Waiting for treedump comparison result ' +
    //           waitCount +
    //           '/' +
    //           maxWait +
    //           '...####'
    //       );
    //       wait(100);
    //       waitCount += 1;
    //     }
    //   } while (waitCount <= maxWait && !testResult);
    //   return testResult;
    // }
    timeoutForPageLoaded(currentTimeout) {
        if (currentTimeout)
            return currentTimeout;
        return this.waitforPageTimeout;
    }
    get loadButton() {
        return By('x_LoadButton');
    }
    get reactControlErrorMessage() {
        return By(REACT_CONTROL_ERROR_TEST_ID);
    }
    isPagedLoadedOrLoadBundleError() {
        if (this.reactControlErrorMessage.isDisplayed()) {
            throw "ReactControl doesn't load bundle successfully: " +
                this.reactControlErrorMessage.getText();
        }
        return this.isPageLoaded();
    }
}
exports.BasePage = BasePage;
//# sourceMappingURL=BasePage.js.map