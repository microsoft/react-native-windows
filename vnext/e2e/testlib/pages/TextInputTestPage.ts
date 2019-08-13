import { TestPage } from './TestPage';
import { WebDriver2, By2 } from 'selenium-appium';
import { until, WebElementCondition, Locator } from 'selenium-webdriver';
import { webDriver2 } from '../testsetup';

export class TextInputTestPage extends TestPage {
  constructor(driver: WebDriver2, timeout?: number) {
    super(driver, timeout);
    this.textInputLocator = By2.nativeAccessibilityId('TextInput');
  }

  textInputLocator: Locator;

  isReadyConditions(): WebElementCondition[] {
    return [until.elementLocated(this.textInputLocator)];
  }

  getTextInput() {
    return webDriver2.findElement(this.textInputLocator);
  }
}