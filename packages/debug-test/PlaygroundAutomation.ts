// Notes about WebDriverIO/WinAppDriver usage in this file:
//
// It seems that the WebDriverIO/WinAppDriver combination is buggy. Among the issues discovered are:
// - WinAppDriver does not support the search strategies resulting from some selector expressions,
//   resulting in errors like
//       ERROR webdriver: Request failed with status 501 due to Error: Unimplemented Command: css selector locator strategy is not supported
// - When using the findElementByXPath method, WebdriverIO (WDIO) finds UI elements after they have been removed.
//   This happens despite WinAppDriver responding with seemingly correct 404 "element not found"
//   errors. The returned UI elements can be queried for their text property, suggesting they represent 
//   WDIO cache state.
// - XPath expressions that yield expected results in $$() selector expressions result in "invalid elementId"
//   errors when used in $() selector expressions.
// - WDIO does not appear to recognize XPath expressions unless they start with either "//" or "./"
// At the time of this writing, the most reliable/predictable way of finding elements seems to be $$() selector
// expressions with XPath arguments. Those expressions return arrays which is the reason why many UI element
// lookups in this file are conducted "in the plural" using array index index expressions.

import {testLog} from './TestLog'
import {sleep} from './TestUtilities'
import {app, AutomationElement} from '@react-native-windows/automation';

async function setCheckedState(checkBox: AutomationElement, checkedState: boolean) {
	const checkBoxName = await checkBox.getText();
	if ((await checkBox.isSelected()) != checkedState) {
		// REVIEW: Does this synthesize an actual click? Do we need to scroll this into view or do some other preparatory steps?
		testLog.message(`changing "${checkBoxName}" checkbox state to ${checkedState}`);
		return checkBox.click();
	}
	testLog.message(`"${checkBoxName}" has checkbox state ${checkedState}`);
}

export async function selectPackage(packageName: string) : Promise<void> {
	testLog.message(`selecting "${packageName}" bundle`);

	const entryPointComboBox = (await $$('./Window/ComboBox[@AutomationId=\"x_entryPointCombo\"]'))[0];
	await entryPointComboBox.setValue(packageName);

	const rootComponentComboBox = (await $$('./Window/ComboBox[@AutomationId=\"x_rootComponentNameCombo\"]'))[0];
	await rootComponentComboBox.click();

	const loadButton = (await $$('./Window/Button[@AutomationId=\"x_LoadButton\"]'))[0];
	await loadButton.click();

	// synchronize with the bundle getting loaded
	await app.waitUntil(async () => {
		const loadingIndicators = await $$('./Window/Text[@Name="Loading bundle."]');

		if (loadingIndicators.length == 0){
			testLog.message(`"${packageName}" bundle appears to have loaded`);
			return true;
		} else {
			testLog.message(`waiting for "${packageName}" bundle to load`);
			return false;
		}
	}, {interval: 2000 /* poll every 2 seconds for the disappearance of the loading indicator */});
}

type DebugSettings = {
	// value representing the "Web Debugger" checkbox
	webDebugger?: boolean;

	// value representing the "Direct Debugging" checkbox
	directDebugging?: boolean;

	// value representing the "Debugger Port" edit
	debuggerPort?: number;

	// value representing the "JS Engine" drop list selection
	jsEngine?: string;
}

export class PlaygroundDebugSettings {
	public constructor(settings: DebugSettings){
		this.newSettings = settings;
	}

	public destructor(){
	}

	public async initialize() {
		await this.fetchSettings(this.oldSettings);
		testLog.message('adjusting settings...');
		await this.adjustSettings(this.newSettings);
		testLog.message('...done adjusting settings');
	}
	
	public async uninitialize() {
		testLog.message('restoring settings...');
		await this.adjustSettings(this.oldSettings);
		testLog.message('...done restoring settings');
	}
	
	private async fetchSettings(settings: DebugSettings){
		const webDebugger = await $$(this.webDebuggerCheckBoxSelector).then(a => a[0].isSelected());
		const directDebugging = await $$(this.directDebuggingCheckBoxSelector).then(a => a[0].isSelected());
		const debuggerPort = await $$(this.debuggerPortEditSelector).then(a => a[0].getText());
		const jsEngine = await $$(this.jsEngineComboBoxSelector).then(a => a[0].getText());

		settings.webDebugger = webDebugger;
		settings.directDebugging = directDebugging;
		settings.debuggerPort = parseInt(debuggerPort);
		settings.jsEngine = jsEngine;
	}
	
	private async adjustSettings(settings: DebugSettings) {
		// Treating the "Web Debugger" check box special as checking it disables the "Debugger Port"
		// edit and the "JS Engine" combo.
		const webDebuggerCheckBox = (await $$(this.webDebuggerCheckBoxSelector))[0];
		let restoreWebDebugger = false;

		if (settings.hasOwnProperty('directDebugging')) {
			const directDebuggingCheckBox = (await $$(this.directDebuggingCheckBoxSelector))[0];
			await setCheckedState(directDebuggingCheckBox, settings.directDebugging);
		}

		if (settings.hasOwnProperty('debuggerPort')) {
			
			const debuggerPortEdit = (await $$(this.debuggerPortEditSelector))[0];
			const currentPortText = await debuggerPortEdit.getText();
	
			if (currentPortText != settings.debuggerPort.toString()){

				if (webDebuggerCheckBox.isSelected()) {
					// enable port number to be changed
					await setCheckedState(webDebuggerCheckBox, false);
					restoreWebDebugger = true; // restore it later (unless it should be off per settings)
				}
				
				await debuggerPortEdit.setValue(settings.debuggerPort);
			}
		}

		const jsEngineComboBox = (await $$(this.jsEngineComboBoxSelector))[0];
		const oldJsEngine = await jsEngineComboBox.getText();
		if (settings.hasOwnProperty('jsEngine')) {
			if (oldJsEngine != settings.jsEngine) {

				// expand the JS engine drop list
				await jsEngineComboBox.click();

				// wait for drop list to expand
				await app.waitUntil(async () => {
					const jsEngineListItems = await $$(`${this.jsEngineComboBoxSelector}/ListItem`);
					return jsEngineListItems.length > 1;
				});

				const jsEngineTargetListItems = await $$(`${this.jsEngineComboBoxSelector}/ListItem[@Name="${settings.jsEngine}"]`);
				await jsEngineTargetListItems[0].click();

				// wait for drop list to collapse
				await app.waitUntil(async () => {
					const jsEngineListItems = await $$(`${this.jsEngineComboBoxSelector}/ListItem`);
					return jsEngineListItems.length <= 1;
				});

				testLog.message(`changed JS engine from "${oldJsEngine}" to "${await jsEngineComboBox.getText()}"`);
			} else {
				testLog.message(`current JS engine matches settings ("${oldJsEngine}")`);
			}
		} else {
			testLog.message(`no JS engine specified, leaving at current setting ("${oldJsEngine}")`)
		}

		if (settings.hasOwnProperty('webDebugger')) {
			await setCheckedState(webDebuggerCheckBox, settings.webDebugger);
			restoreWebDebugger = false;
		}

		if (restoreWebDebugger) {
			await setCheckedState(webDebuggerCheckBox, true);
		}
	}

	private oldSettings : DebugSettings = {};
	private newSettings : DebugSettings;

	private webDebuggerCheckBoxSelector = './Window/CheckBox[@AutomationId=\"x_UseWebDebuggerCheckBox\"]';
	private directDebuggingCheckBoxSelector = './Window/CheckBox[@AutomationId=\"x_UseDirectDebuggerCheckBox\"]';
	private debuggerPortEditSelector = './Window/Edit[@AutomationId=\"x_DebuggerPort\"]';
	private jsEngineComboBoxSelector = './Window/ComboBox[@AutomationId="x_JsEngine"]';
}

async function exploreUI() {
	// const r01 = await $$('Window/Button'); // Unimplemented Command: css selector locator strategy is not supported
	// testLog.message(`r01.length=${r01.length}`);

	// const r02 = await $$('/Window/Button'); // r02.length = 0
	// testLog.message(`r02.length = ${r02.length}`);

	// const r03 = await $$('./Window/Button'); // r03.length = 5
	// testLog.message(`r03.length = ${r03.length}`);
	// for(const e of r03) {
	// 	const t = await e.getText();
	// 	testLog.message(`r03 element ${t}`);
	// }
	// =>
	// r03.length = 5
	// r03 element Minimize playground // title bar (title bar has control type Window)
	// r03 element Maximize playground
	// r03 element Close playground
	// r03 element Load
	// r03 element Unload


	const jsEngineComboBoxes = await $$('./Window/ComboBox[@AutomationId="x_JsEngine"]');
	testLog.message(`jsEngineComboBoxes.length ${jsEngineComboBoxes.length}`);

	testLog.message("before getting text");
	const jsEngine1 = await jsEngineComboBoxes[0].getText();
	testLog.message("after getting text");
	testLog.message(`jsEngineCombo before: "${jsEngine1}"`);


	const r04 = await $$('./Window/ComboBox[@AutomationId="x_JsEngine"]/ListItem');
	testLog.message(`r04.length = ${r04.length}`);
	jsEngineComboBoxes[0].click(); // expand
	await sleep(1000);
	const r05 = await $$('./Window/ComboBox[@AutomationId="x_JsEngine"]/ListItem');
	testLog.message(`r05.length = ${r05.length}`);
	for(const e of r05) {
		const t = await e.getText();
		testLog.message(`r05 element ${t}`);
	}

	jsEngineComboBoxes[0].click(); // collapse
	await sleep(1000);
	const r06 = await $$('./Window/ComboBox[@AutomationId="x_JsEngine"]/ListItem');
	testLog.message(`r06.length = ${r06.length}`);
	for(const e of r06) {
		const t = await e.getText();
		testLog.message(`r06 element ${t}`);
	}
}

