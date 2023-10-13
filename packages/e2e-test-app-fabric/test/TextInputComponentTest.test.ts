/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('TextInput');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('ButtonTests', () => {
  test('TextInputs can rewrite characters: Replace Space with Underscore', async () => {
    const dump = await dumpVisualTree('rewrite_sp_underscore_input');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can rewrite characters: Replace Space with No Character', async () => {
    const dump = await dumpVisualTree('rewrite_no_sp_input');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can rewrite characters: Replace Space with Clear', async () => {
    const dump = await dumpVisualTree('rewrite_clear_input');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Turned Off', async () => {
    const dump = await dumpVisualTree('capitalize-none');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Sentences', async () => {
    const dump = await dumpVisualTree('capitalize-sentences');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Words', async () => {
    const dump = await dumpVisualTree('capitalize-words');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocapitalize: Autocapitalize Characters', async () => {
    const dump = await dumpVisualTree('capitalize-characters');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have attributed text', async () => {
    const dump = await dumpVisualTree('text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Single-line TextInputs can enable text selection', async () => {
    const dump = await dumpVisualTree('singleline-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Multi-line TextInputs can enable text selection', async () => {
    const dump = await dumpVisualTree('multiline-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Single-line TextInputs can enable text selection (Imperative)', async () => {
    const dump = await dumpVisualTree('singlelineImperative-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Multi-line TextInputs can enable text selection (Imperative)', async () => {
    const dump = await dumpVisualTree('multilineImperative-text-input');
    expect(dump).toMatchSnapshot();
  });
  test('Uncontrolled TextInput', async () => {
    const dump = await dumpVisualTree('uncontrolled-textinput');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a background color', async () => {
    const dump = await dumpVisualTree('style-backgroundColor');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a color', async () => {
    const dump = await dumpVisualTree('style-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font family', async () => {
    const dump = await dumpVisualTree('style-fontFamily');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font size', async () => {
    const dump = await dumpVisualTree('style-fontSize');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font style', async () => {
    const dump = await dumpVisualTree('style-fontStyle');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have a font weight', async () => {
    const dump = await dumpVisualTree('style-fontWeight');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized letter spacing', async () => {
    const dump = await dumpVisualTree('style-letterSpacing');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized line height', async () => {
    const dump = await dumpVisualTree('style-lineHeight');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have text decoration lines', async () => {
    const dump = await dumpVisualTree('style-textDecorationLine');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have text shadows', async () => {
    const dump = await dumpVisualTree('style-textShadow');
    expect(dump).toMatchSnapshot();
  });
});
