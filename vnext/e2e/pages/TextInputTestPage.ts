/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage } from './BasePage';

class TextInputTestPage extends BasePage {
	get textInput() {
		return browser.$('~TextInput');
	}

	isPageLoaded() {
		return super.isPageLoaded() && this.textInput.isDisplayed();
	}

	clearAndTypeOnTextInput(text: string) {
		this.textInput.setValue(text);
	}

	getTextInputText() {
		this.textInput.getText();
	}
}

export default new TextInputTestPage();
