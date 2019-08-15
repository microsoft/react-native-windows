/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import TextInputTestPage from '../pages/TextInputTestPage';
import HomePage from '../pages/HomePage';
import assert from 'assert';

describe('First', () => {
	beforeEach(() => {
		HomePage.backToHomePage();
		HomePage.clickAndGoToTextInputPage();
	});

	it('Type abc on TextInput', () => {
		TextInputTestPage.clearAndTypeOnTextInput('abc');
		assert.equal(TextInputTestPage.getTextInputText(), 'abc');
	});

	it('Type def on TextInput', () => {
		TextInputTestPage.clearAndTypeOnTextInput('def');
		assert.equal(TextInputTestPage.getTextInputText(), 'def');
	});

	afterEach(() => {
		HomePage.backToHomePage();
	});
});

describe('Second', () => {
	beforeEach(() => {
		HomePage.backToHomePage();
		HomePage.clickAndGoToTextInputPage();
	});

	it('Type abc on TextInput', () => {
		TextInputTestPage.clearAndTypeOnTextInput('abc');
		assert.equal(TextInputTestPage.getTextInputText(), 'abc');
	});

	it('Type def on TextInput', () => {
		TextInputTestPage.clearAndTypeOnTextInput('def');
		assert.equal(TextInputTestPage.getTextInputText(), 'def');
	});

	afterEach(() => {
		HomePage.backToHomePage();
	});
});
