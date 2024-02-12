/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @file Contains code to automate the RNW UWP Playground application.
 *
 * @format
 */

// Notes about WebDriverIO/WinAppDriver usage in this file:
//
// It seems that the WebDriverIO/WinAppDriver combination is buggy. Among the issues discovered are:
// - WinAppDriver does not support the search strategies resulting from some selector expressions,
//   resulting in errors like
//       ERROR webdriver: Request failed with status 501 due to Error: Unimplemented Command: css selector locator strategy is not supported
// - When using the findElementByXPath method, WebdriverIO (WDIO) finds UI elements after they have been removed.
//   This happens despite WinAppDriver responding with seemingly correct 404 "element not found"
//   errors. The returned UI elements can be queried for their text property, suggesting they represent
//   WDIO cache state.
// - XPath expressions that yield expected results in $$() selector expressions result in "invalid elementId"
//   errors when used in $() selector expressions.
// - WDIO does not appear to recognize XPath expressions unless they start with either "//" or "./"
// At the time of this writing, the most reliable/predictable way of finding elements seems to be $$() selector
// expressions with XPath arguments. Those expressions return arrays which is the reason why many UI element
// lookups in this file are conducted "in the plural" using array index index expressions.

import {testLog} from './TestLog';
import {app, AutomationElement} from '@react-native-windows/automation';
import {saveScreenShot, sleep} from './Utilities';

async function setCheckedState(
  checkBox: AutomationElement,
  checkedState: boolean,
) {
  const checkBoxName = await checkBox.getText();
  if ((await checkBox.isSelected()) !== checkedState) {
    // REVIEW: Does this synthesize an actual click? Do we need to scroll this into view or do some other preparatory steps?
    testLog.message(
      `changing "${checkBoxName}" checkbox state to ${checkedState}`,
    );
    return checkBox.click();
  }
  testLog.message(`"${checkBoxName}" has checkbox state ${checkedState}`);
}

export async function primePackageComboBox() {
  // On ADO agents, initial expansion of the drop list can result in a UIA tree that's missing nodes for
  // some list items. We're trying to "prime" the drop list by dropping and collapsing it prior to test
  // execution.
  testLog.message('start primePackageComboBox');
  const entryPointComboBox = (
    await $$('//Window/ComboBox[@AutomationId="x_entryPointCombo"]')
  )[0];

  // found to be necessary in ADO runs - perhaps it puts the app into the foreground
  await entryPointComboBox.click();
  await sleep(500);

  await entryPointComboBox.click(); // focus combo box
  await sleep(500);

  await entryPointComboBox.keys(['ArrowDown']); // drop drop list
  await sleep(500);
  await entryPointComboBox.keys(['End']); // move cursor to end of edit box
  await sleep(500);
  await entryPointComboBox.keys(['End']); //select last drop list item
  await sleep(500);

  await app.waitUntil(
    async () =>
      (
        await $$(
          '//Window/ComboBox[@AutomationId="x_entryPointCombo"]/ListItem',
        )
      ).length > 1,
    {timeout: 20000},
  );

  const listItems = await $$(
    '//Window/ComboBox[@AutomationId="x_entryPointCombo"]/ListItem',
  );

  for (let i = 0; i < listItems.length; ++i) {
    const listItemText = await listItems[i].getText();
    testLog.message(`text of list item ${i}: "${listItemText}"`);
  }
  await saveScreenShot('After Initial Combo Box Expansion');

  await entryPointComboBox.keys(['Home']); // move cursor to start of edit box
  await sleep(500);
  await entryPointComboBox.keys(['Home']); // select first drop list item
  await sleep(500);
  await entryPointComboBox.keys(['Tab']); // collapse drop list
  await sleep(500);

  await saveScreenShot('After Initial Combo Box Collapse');
  testLog.message('end primePackageComboBox');
}

async function selectPackage(packageName: string) {
  // It was hard to get this operation to succeed on ADO agents. The fundamental problem appears to be
  // that UIA interactions - which are inherently asynchronous - seem to execute much slower
  // on ADO agents. This requires synchronization "checkpoints" that are hard to implement due to
  // - missing WebDriver API elements (mainly events)
  // - missing WinAppDriver implementations of WebDriver commands
  // - peculiarities of the UIA implementation of the XAML combo box causing WinAppDriver commands
  //   to behave in unexpected ways.
  // Many alternatives have been tried; the steps below are the ones that were found to be working.
  // The frailty of this indicates that we should probably switch from a WDIO-based automation solution
  // to an automation channel that is simpler and our under our full control.
  //
  // Findings:
  // - combo box setValue() used to work but is now broken in that subsequent calls drop the first letter
  // - combo box does not support isFocused()
  // - combo box does not support getText()
  // - combo box does not support getValue()
  // - combo box value can be retrieved via getText() on a nested edit control, but the edit control
  //   may only be present when keyboard focus is in the combo box
  // - upon initial expansion of the drop list, not all UIA nodes for the list items may get generated
  // - selectByVisibleText() on the combo box does not work (perhaps it only fails for the last item
  //   for the reason listed above, or because the list item is scrolled out of view and marked off-screen)

  testLog.message(`start selecting package "${packageName}"`);

  const entryPointComboBox = (
    await $$('//Window/ComboBox[@AutomationId="x_entryPointCombo"]')
  )[0];

  // expand drop list
  await entryPointComboBox.click(); // focus drop list
  await entryPointComboBox.keys(['ArrowDown']); // expand drop list

  // synchronize with the appearance of the drop list
  // When collapsed, there's one list item (the currently selected one). To
  // sync with the expansion of the drop list, we need to test for more than
  // one list item. We would need to restructure the algorithm if there is
  // only one list item (no selection change required).
  await app.waitUntil(
    async () =>
      (
        await $$(
          './Window/ComboBox[@AutomationId="x_entryPointCombo"]/ListItem',
        )
      ).length > 1,
    {timeout: 10000},
  );
  await saveScreenShot('After Dropping Combo Box');

  const listItems = await $$(
    '//Window/ComboBox[@AutomationId="x_entryPointCombo"]/ListItem',
  );
  testLog.message(`list item count: ${listItems.length}`);

  let currentIndex = -1;
  let targetIndex = -1;

  for (let i = 0; i < listItems.length; ++i) {
    const listItemText = await listItems[i].getText();
    testLog.message(`text of list item ${i}: "${listItemText}"`);
    if (await listItems[i].isSelected()) currentIndex = i;
    if (listItemText === packageName) targetIndex = i;
    if (currentIndex !== -1 && targetIndex !== -1) break;
  }

  if (currentIndex === -1)
    throw new Error(`can't find currently selected drop list item`);
  if (targetIndex === -1) throw new Error(`can't find target drop list item`);

  testLog.message(`currentIndex: ${currentIndex}`);
  testLog.message(`targetIndex: ${targetIndex}`);

  if (targetIndex > currentIndex) {
    for (let i = 0; i < targetIndex - currentIndex; ++i) {
      await entryPointComboBox.keys(['ArrowDown']);
      await sleep(100);
    }
  } else if (targetIndex < currentIndex) {
    for (let i = 0; i < currentIndex - targetIndex; ++i) {
      await entryPointComboBox.keys(['ArrowUp']);
      await sleep(100);
    }
  }

  // select the item and collapse the drop list
  await entryPointComboBox.keys(['Return']);

  // synchronize with the collapse of the drop list
  await app.waitUntil(
    async () =>
      (
        await $$(
          './Window/ComboBox[@AutomationId="x_entryPointCombo"]/ListItem',
        )
      ).length <= 1,
    {timeout: 10000},
  );
  testLog.message(`done selecting package "${packageName}"`);
  await saveScreenShot('After Selecting Combo Box Item');
}

/**
 * Selects (loads) a package in the Playground app.
 *
 * @param packageName Name of the package to load.
 * @returns Promise indicating that the package was loaded.
 */
export async function loadPackage(
  packageName: string,
  bundleServed: Promise<void>,
): Promise<void> {
  testLog.message(`selecting "${packageName}" bundle`);

  await selectPackage(packageName);

  const loadButton = (
    await $$('./Window/Button[@AutomationId="x_LoadButton"]')
  )[0];
  await loadButton.click();
  testLog.message(`clicked on load button`);

  await bundleServed;
  testLog.message(`Metro appears to have served the bundle`);

  // synchronize with the bundle getting loaded
  await app.waitUntil(
    async () => {
      const loadingIndicators = await $$(
        './Window/Text[@Name="Loading bundle."]',
      );

      if (loadingIndicators.length === 0) {
        testLog.message(`"${packageName}" bundle appears to have loaded`);
        return true;
      } else {
        testLog.message(`waiting for "${packageName}" bundle to load`);
        return false;
      }
    },
    {
      interval: 2000 /* poll every 2 seconds for the disappearance of the loading indicator */,
    },
  );
}

/**
 * Debug settings for the Playground app.
 */
export type DebugSettings = {
  // value representing the "Direct Debugging" checkbox
  directDebugging?: boolean;

  // value representing the "Debugger Port" edit
  debuggerPort?: number;

  // value representing the "JS Engine" drop list selection
  jsEngine?: string;
};

/**
 * RAII-style class to set and restore debug settings in the Playground app.
 */
export class PlaygroundDebugSettings {
  private constructor(settings: DebugSettings) {
    this.newSettings = settings;
  }

  /**
   * Saves the current debug settings and effects the desired debug settings.
   * @param settings Desired debug settings.
   */
  public static async set(
    settings: DebugSettings,
  ): Promise<PlaygroundDebugSettings> {
    const obj = new PlaygroundDebugSettings(settings);
    await obj.fetchSettings(obj.oldSettings);
    testLog.message('adjusting settings...');
    await obj.adjustSettings(obj.newSettings);
    testLog.message('...done adjusting settings');
    return obj;
  }

  /**
   * Restores initial debug settings.
   */
  public async restore() {
    testLog.message('restoring settings...');
    await this.adjustSettings(this.oldSettings);
    testLog.message('...done restoring settings');
  }

  private async fetchSettings(settings: DebugSettings) {
    const directDebugging = await $$(this.directDebuggingCheckBoxSelector).then(
      a => a[0].isSelected(),
    );
    const debuggerPort = await $$(this.debuggerPortEditSelector).then(a =>
      a[0].getText(),
    );
    const jsEngine = await $$(this.jsEngineComboBoxSelector).then(a =>
      a[0].getText(),
    );

    settings.directDebugging = directDebugging;
    settings.debuggerPort = parseInt(debuggerPort, 10);
    settings.jsEngine = jsEngine;
  }

  private async adjustSettings(settings: DebugSettings) {
    if (settings.directDebugging !== undefined) {
      const directDebuggingCheckBox = (
        await $$(this.directDebuggingCheckBoxSelector)
      )[0];
      await setCheckedState(directDebuggingCheckBox, settings.directDebugging);
    }

    if (settings.debuggerPort !== undefined) {
      const debuggerPortEdit = (await $$(this.debuggerPortEditSelector))[0];
      const currentPortText = await debuggerPortEdit.getText();

      if (currentPortText !== settings.debuggerPort.toString()) {
        await debuggerPortEdit.setValue(settings.debuggerPort);
      }
    }

    const jsEngineComboBox = (await $$(this.jsEngineComboBoxSelector))[0];
    const oldJsEngine = await jsEngineComboBox.getText();
    if (settings.jsEngine !== undefined) {
      if (oldJsEngine !== settings.jsEngine) {
        // expand the JS engine drop list
        await jsEngineComboBox.click();

        // wait for drop list to expand
        await app.waitUntil(async () => {
          const jsEngineListItems = await $$(
            `${this.jsEngineComboBoxSelector}/ListItem`,
          );
          return jsEngineListItems.length > 1;
        });

        const jsEngineTargetListItems = await $$(
          `${this.jsEngineComboBoxSelector}/ListItem[@Name="${settings.jsEngine}"]`,
        );
        await jsEngineTargetListItems[0].click();

        // wait for drop list to collapse
        await app.waitUntil(async () => {
          const jsEngineListItems = await $$(
            `${this.jsEngineComboBoxSelector}/ListItem`,
          );
          return jsEngineListItems.length <= 1;
        });

        testLog.message(
          `changed JS engine from "${oldJsEngine}" to "${await jsEngineComboBox.getText()}"`,
        );
      } else {
        testLog.message(
          `current JS engine matches settings ("${oldJsEngine}")`,
        );
      }
    } else {
      testLog.message(
        `no JS engine specified, leaving at current setting ("${oldJsEngine}")`,
      );
    }
  }

  private readonly oldSettings: DebugSettings = {};
  private readonly newSettings: DebugSettings;

  private readonly directDebuggingCheckBoxSelector =
    './Window/CheckBox[@AutomationId="x_UseDirectDebuggerCheckBox"]';
  private readonly debuggerPortEditSelector =
    './Window/Edit[@AutomationId="x_DebuggerPort"]';
  private readonly jsEngineComboBoxSelector =
    './Window/ComboBox[@AutomationId="x_JsEngine"]';
}
