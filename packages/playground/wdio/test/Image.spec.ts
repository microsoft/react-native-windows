/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

// import HomePage from '../pages/HomePage';
// import { By } from '../pages/BasePage';
// import ImageTestPage from '../pages/ImageTestPage';
import assert from 'assert';
import WebdriverIO from '@wdio/sync';
// import WebdriverIO from 'webdriverio';

function VerifyTreeDumpOk(pageName : string) {
  browser.waitUntil(() => $('~x_TreeDump').getText() == 'OK', 15000, `ERROR - treedump comparison failed for page ${pageName}`);
  const treedump = $('~x_TreeDump').getText();
  assert(treedump == 'OK', `treedump = ${treedump} on page ${pageName}`);
}

function VerifyPage(element : WebdriverIO.Element) {
    const name = element.getText();
  element.click();
  browser.waitUntil(() => $('~PageHeader').getText() == name, 10000, `Timeout waiting for page ${name} to load, pageheader was ${$('~PageHeader').getText()}`);
  browser.pause(2000); // wait 2 seconds for the page header to refresh post-navigation

  const dismissYellowBox = $('/Window/Window[2]/Custom/Text');
    if (dismissYellowBox && dismissYellowBox.isDisplayed()) {
        dismissYellowBox.waitForEnabled(1000);
    dismissYellowBox.click();
  }
  VerifyTreeDumpOk(name);
  const backButton = $('/Window/Window[2]/Button[2]');
  backButton.waitForEnabled();
  backButton.click();
}

beforeAll(() => {
  // HomePage.loadRNTester();
  // Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, 3000);
  // HomePage.waitForPageLoaded();
  // HomePage.clickAndGotoImagePage();
});

describe('basicTest', () => {
  it('basicTest', async () => {
    browser.maximizeWindow();
    $('~x_LoadButton').waitForEnabled(30000, false, "TIMEOUT. NOT ENABLED"); //.findElement('accessibility id', 'x_LoadButton'), 30000, "TIMEOUT 30s");// $('~x_LoadButton') != undefined);// HomePage.isPageLoaded);
    const load = $('~x_LoadButton');
    load.click();
    $('~PageHeader').waitForDisplayed(30000, false, 'No pageheader');
    const title = $('~PageHeader').getText();
    assert(title == 'RNTester');

    VerifyTreeDumpOk('RNTester');

    const activityIndicator = $('/Window/Window[2]/Pane[1]/Text[2]');
    activityIndicator.waitForEnabled(30000, false, 'No ActivityIndicator');
    assert(activityIndicator.getText() == '<ActivityIndicator>' , `text = ${activityIndicator.getText()}`);
    
    const paneItems = $$('/Window/Window[2]/Pane[1]/Text');
    const numberOfPages = paneItems.length;
    assert($('/Window/Window[2]/Pane[1]/Text[1]').getText() == 'COMPONENTS');

      const allowedPages = [
          'RNTester',
          '<ActivityIndicator>',
          '<Button>',
          '<CheckBox>',
          '<DatePicker>',
          'Fast Path Texts',
          // '<FlatList>',
          '<Flyout>',
          '<Glyph> UWP',
          // #4691   '<Image>',
      ];
    for (let i = 2; i < numberOfPages; i += 2) {
      const page = $(`/Window/Window[2]/Pane[1]/Text[${i}]`);
      if (allowedPages.find((x) => x == page.getText())) {
        VerifyPage(page);
      }
    }

  });
});
// describe('ImageWithoutBorderTest', () => {
//   /* Test case #1: view and image displayed with no border and cornerRadius */
//   it('ImageWithoutBorderTest', () => {
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#1. Dump comparison for image without border!');
//   });

//   /* Test case #2: Click button once, update view and image with round border*/
//   it('ImageWithBorderTest', () => {
//     ImageTestPage.toggleImageBorder();
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#2. Dump comparison for image with border!');
//   });

//   /* Test case #3: Click button one more, remove border from view and image but tree sturcture is different from #1*/
//   it('ImageWithoutBorderTest', () => {
//     ImageTestPage.toggleImageBorder();
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#3. Second dump comparison for image without border!');
//   });

//   it('ImageRTLTest', () => {
//     ImageTestPage.toggleRTLMode();
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#4. Dump comparison for image RTL');
//   });
// });
