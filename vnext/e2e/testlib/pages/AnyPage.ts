import { PageObject, WebDriver2 } from "selenium-appium";
import { WebElementCondition, until } from "selenium-webdriver";
import { Locators } from "./Locators";

export class AnyPage extends PageObject {
  isReadyConditions(): WebElementCondition[] {
    return [until.elementLocated(Locators.homeLocator)];
  }

  isOnTestPage(): Promise<boolean> {
     return this.elementExists(Locators.backButtonLocator);
  }

  isOnReactErrorPage(): Promise<boolean> {
    return this.elementExists(Locators.reactControlErrorBoxLocator);    
  }

  constructor(driver: WebDriver2, timeout?: number) {
    super(driver, timeout)
  }
}