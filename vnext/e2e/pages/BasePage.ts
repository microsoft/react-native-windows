/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

export class BasePage {
	get reactControlErrorMessageBox() {
		return $('~_ReactControlErrorBox');
	}
	// Default timeout for waitForPageLoaded command in PageObject
	private waitforPageTimeout: number = 10000;

	protected waitTimeoutForPageLoaded(currentTimeout?: number) {
		if (currentTimeout) return currentTimeout;
		return this.waitforPageTimeout;
	}

	get homeButton() {
		return $('~__HomeButton');
	}

	isPageLoaded(): boolean {
		return this.homeButton.isDisplayed();
	}

	isPagedLoadedOrFailBecauseReactControlError(): boolean {
		if (this.reactControlErrorMessageBox.isDisplayed()) {
			throw "ReactControl doesn't load bundle successfully. Possible Metro Bundler is not launched, or errors in launch bundle";
		}
		return this.isPageLoaded();
	}

	waitForPageLoaded(timeout?: number) {
		browser.waitUntil(
			() => {
				return this.isPagedLoadedOrFailBecauseReactControlError();
			},
			this.waitTimeoutForPageLoaded(timeout),
			'Wait for page ' + this.constructor.name + ' timeout'
		);
	}
}
