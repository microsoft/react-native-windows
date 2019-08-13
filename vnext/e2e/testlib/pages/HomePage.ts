import { PageObject, By2, WebDriver2, IPageObject } from 'selenium-appium';
import { WebElementCondition, until, By } from 'selenium-webdriver';
import { TextInputTestPage } from './TextInputTestPage';
import { Locators } from './Locators'
import { AnyPage } from './AnyPage';

export class HomePage extends PageObject {
  isReadyConditions(): WebElementCondition[] {
    return [until.elementLocated(By2.nativeAccessibilityId('__HomeButton'))];
  }

  clickAndWaitForTextInputTestPage() {
    return this.clickAndWaitForPage(TextInputTestPage, Locators.textInputTestPageLocator);
  }

  constructor(driver: WebDriver2, timeout?: number) {
    super(driver, timeout)
  }
}

export function clickAndWaitForHomePage(driver: WebDriver2, by?: By, timeout?: number) {
  if (!by) {
    by = Locators.homeLocator;
  }
  return clickAndWaitForPage(HomePage, driver, by, timeout);
}

export function clickAndWaitForPage<T extends IPageObject>(type: (new (...args: any[]) => T), driver: WebDriver2, by: By, timeout?: number) {
  return new AnyPage(driver).clickAndWaitForPage(type, by, timeout);
}