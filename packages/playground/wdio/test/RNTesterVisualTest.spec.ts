/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import assert from 'assert';
import WebdriverIO from '@wdio/sync';

function VerifyTreeDumpOk(pageName : string) {
  browser.waitUntil(() => $('~x_TreeDump').getText() == 'OK', 15000, `ERROR - treedump comparison failed for page ${pageName}`);
  const treedump = $('~x_TreeDump').getText();
  assert(treedump == 'OK', `treedump = ${treedump} on page ${pageName}`);
}

function DismissYellowBox() {
    const dismissYellowBox = $('/Window/Window[2]/Custom/Text');
    if (dismissYellowBox && dismissYellowBox.isDisplayed()) {
        dismissYellowBox.waitForEnabled(1000);
        dismissYellowBox.click();
    }
}

function VerifyPage(element : WebdriverIO.Element) {
  const name = element.getText();
  element.click();
  browser.waitUntil(() => $('~PageHeader').getText() == name, 10000, `Timeout waiting for page ${name} to load, pageheader was ${$('~PageHeader').getText()}`);
  browser.pause(2000); // wait 2 seconds for the page header to refresh post-navigation

  DismissYellowBox();
    VerifyTreeDumpOk(name);
  const backButton = $('/Window/Window[2]/Button[2]');
  backButton.waitForEnabled();
  backButton.click();
}

beforeAll(() => {
    browser.maximizeWindow();
});

function findPage(name: String) {
    const paneItems = $$('/Window/Window[2]/Pane[1]/Text');
    return paneItems.find(x => x.getText() == name);
}

describe('RNTesterVisualTests', () => {
    it('loadTest', async () => {
        $('~x_LoadButton').waitForEnabled(30000, false, "TIMEOUT. NOT ENABLED");
        const load = $('~x_LoadButton');
        load.click();
        $('~PageHeader').waitForDisplayed(30000, false, 'No pageheader');
        const title = $('~PageHeader').getText();
        assert(title == 'RNTester');

        VerifyTreeDumpOk('RNTester');
        DismissYellowBox();
    });
    it('basicSanity', async () => {
        const activityIndicator = $('/Window/Window[2]/Pane[1]/Text[2]');
        activityIndicator.waitForEnabled(30000, false, 'No ActivityIndicator');
        assert(activityIndicator.getText() == '<ActivityIndicator>', `text = ${activityIndicator.getText()}`);
        assert($('/Window/Window[2]/Pane[1]/Text[1]').getText() == 'COMPONENTS');
    });

    const allowedPages = [
        '123<ActivityIndicator>',
        '<Button>',
        '<CheckBox>',
        '<DatePicker>',
        'Fast Path Texts',
        // TODO: Virtualization is making FlatList treedump fail  // '<FlatList>',
        // TODO: At 1280x1024, the following elements are beyond the view and so clicking fails.
        // This would normally be solved by scrollIntoView() but that method throws a not implemented exception
        // You can uncomment these and manually scroll the elements into view to validate that their masters match
        /*
        '<Flyout>',
        '<Glyph> UWP',
        // #4691 - Image uses resizeMode=repeat which asserts and crashes in Debug
        // '<Image>',
        */
    ];
    for (let i = 0; i < allowedPages.length; i++) {
        it(allowedPages[i], async () => {
            const page = findPage(allowedPages[i]);
            assert(page, `Page ${allowedPages[i]} not found`);
            if (page != undefined) {
                VerifyPage(page);
            }
        });
    }
});


