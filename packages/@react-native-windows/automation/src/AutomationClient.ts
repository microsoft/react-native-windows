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

  /**
   * Resizes app window outer size according to provided width and height.
   */
  setWindowSize: (width: number, height: number) =>
    browser.setWindowSize(width, height),

  /**
   * Change the position of the current focussed window.
   */
  setWindowPosition: (x: number, y: number) => browser.setWindowPosition(x, y),

  /**
   * Get the position of the current focussed window.
   */
  getWindowPosition: () => browser.getWindowPosition(),

  /**
   * Returns app window size.
   */
  getWindowSize: () => browser.getWindowSize(),

  /**
   * Switch focus to a particular window.
   */
  switchWindow: (titleToMatch: string | RegExp) =>
    browser.switchWindow(titleToMatch),

  /**
   * This wait command is your universal weapon if you want to wait on something. It expects a condition
   * and waits until that condition is fulfilled with a truthy value.
   */
  waitUntil: (
    condition: () => Promise<boolean>,
    options?: WebdriverIO.WaitUntilOptions,
  ) => browser.waitUntil(condition, options),
};
