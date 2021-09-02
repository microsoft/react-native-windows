/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/* global $:false, browser:false */

/**
 * Projection of a WebDriver Element, with functions corresponding to supported
 * WinAppDriver APIs.
 *
 * See https://github.com/microsoft/WinAppDriver/blob/master/Docs/SupportedAPIs.md
 */
export type AutomationElement = Pick<
  WebdriverIO.Element,
  | 'addValue'
  | 'clearValue'
  | 'click'
  | 'doubleClick'
  | 'getAttribute'
  | 'getLocation'
  | 'getSize'
  | 'getText'
  | 'getValue'
  | 'isDisplayed'
  | 'isDisplayedInViewport'
  | 'isEnabled'
  | 'isEqual'
  | 'isSelected'
  | 'moveTo'
  | 'saveScreenshot'
  | 'setValue'
  | 'touchAction'
  | 'waitForDisplayed'
  | 'waitForExist'
>;

/**
 * A subset of WebDriver functionality that will work with Windows applications
 */
export const app = {
  /**
   * Find an element by testID property
   */
  findElementByTestID: (id: string): Promise<AutomationElement> => $(`~${id}`),

  /**
   * Find an element by Automation ID
   *
   * https://docs.microsoft.com/en-us/dotnet/api/system.windows.automation.automationelement.automationelementinformation.automationid?view=net-5.0
   */
  findElementByAutomationID: (id: string): Promise<AutomationElement> =>
    $(`~${id}`),

  /**
   * Finds an element by the name of its class name (e.g. ListViewItem)
   *
   * https://docs.microsoft.com/en-us/dotnet/api/system.windows.automation.automationelement.automationelementinformation.classname?view=net-5.0
   */
  findElementByClassName: (className: string): Promise<AutomationElement> =>
    $(className),

  /**
   * Find element by ControlType (e.g. Button, CheckBox)
   *
   * https://docs.microsoft.com/en-us/dotnet/api/system.windows.automation.controltype?view=net-5.0
   */
  findElementByControlType: (controlType: string): Promise<AutomationElement> =>
    $(`<${controlType} />`),

  /**
   * Find element by a WinAppDriver compatible XPath Selector (e.g. '//Button[@AutomationId=\"MoreButton\"]')
   */
  findElementByXPath: (xpath: string): Promise<AutomationElement> => $(xpath),

  setWindowSize: browser.setWindowSize.bind(browser),
  setWindowPosition: browser.setWindowPosition.bind(browser),
  getWindowPosition: browser.getWindowPosition.bind(browser),
  getWindowSize: browser.getWindowSize.bind(browser),
  switchWindow: browser.switchWindow.bind(browser),

  waitUntil: browser.waitUntil.bind(browser),
};
