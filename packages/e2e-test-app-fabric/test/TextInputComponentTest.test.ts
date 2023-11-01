/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await goToComponentExample('TextInput');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('TextInput Tests', () => {
  test('TextInputs can rewrite characters: Replace Space with Underscore', async () => {
    const component = await app.findElementByTestID(
      'rewrite_sp_underscore_input',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('rewrite_sp_underscore_input');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can rewrite characters: Replace Space with No Character', async () => {
    const component = await app.findElementByTestID('rewrite_no_sp_input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('rewrite_no_sp_input');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can rewrite characters: Replace Space with Clear', async () => {
    const component = await app.findElementByTestID('rewrite_clear_input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('rewrite_clear_input');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Turned Off', async () => {
    const component = await app.findElementByTestID('capitalize-none');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-none');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Sentences', async () => {
    const component = await app.findElementByTestID('capitalize-sentences');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-sentences');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Words', async () => {
    const component = await app.findElementByTestID('capitalize-words');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-words');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Characters', async () => {
    const component = await app.findElementByTestID('capitalize-characters');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-characters');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have attributed text', async () => {
    const component = await app.findElementByTestID('text-input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Single-line TextInputs can enable text selection', async () => {
    const component = await app.findElementByTestID('singleline-text-input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('singleline-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Multi-line TextInputs can enable text selection', async () => {
    const component = await app.findElementByTestID('multiline-text-input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('multiline-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Single-line TextInputs can enable text selection (Imperative)', async () => {
    const component = await app.findElementByTestID(
      'singlelineImperative-text-input',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('singlelineImperative-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Multi-line TextInputs can enable text selection (Imperative)', async () => {
    const component = await app.findElementByTestID(
      'multilineImperative-text-input',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('multilineImperative-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Uncontrolled TextInput', async () => {
    const component = await app.findElementByTestID('uncontrolled-textinput');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('uncontrolled-textinput');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a background color', async () => {
    const component = await app.findElementByTestID('style-backgroundColor');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-backgroundColor');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a color', async () => {
    const component = await app.findElementByTestID('style-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font family', async () => {
    const component = await app.findElementByTestID('style-fontFamily');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-fontFamily');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font size', async () => {
    const component = await app.findElementByTestID('style-fontSize');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-fontSize');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font style', async () => {
    const component = await app.findElementByTestID('style-fontStyle');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-fontStyle');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font weight', async () => {
    const component = await app.findElementByTestID('style-fontWeight');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-fontWeight');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized letter spacing', async () => {
    const component = await app.findElementByTestID('style-letterSpacing');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-letterSpacing');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized line height', async () => {
    const component = await app.findElementByTestID('style-lineHeight');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-lineHeight');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have text decoration lines', async () => {
    const component = await app.findElementByTestID('style-textDecorationLine');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-textDecorationLine');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have text shadows', async () => {
    const component = await app.findElementByTestID('style-textShadow');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-textShadow');
    expect(dump).toMatchSnapshot();
  });
});
