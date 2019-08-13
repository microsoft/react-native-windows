import { windowsNativeAppCapabilities, createWebDriver2 } from 'selenium-appium'

const appName = 'ReactUWPTestApp_2wtq0zq3ec38a!App';

export const webDriver2 = createWebDriver2(windowsNativeAppCapabilities(appName));

