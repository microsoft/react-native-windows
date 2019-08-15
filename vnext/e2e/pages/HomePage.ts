/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage } from './BasePage';
import TextInputTestPage from './TextInputTestPage';

class HomePage extends BasePage {
	get testInputTestPageButton() {
		return browser.$('~TextInputTestPage');
	}

	backToHomePage() {
		this.homeButton.click();
		this.waitForPageLoaded();
	}

	isPageLoaded() {
		return super.isPageLoaded() && this.testInputTestPageButton.isDisplayed();
	}

	clickAndGoToTextInputPage() {
		this.testInputTestPageButton.click();
		TextInputTestPage.waitForPageLoaded();
	}
}

export default new HomePage();
