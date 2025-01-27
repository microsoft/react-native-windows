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
  await app.setWindowSize(1000, 1250);
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
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'Hello_World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    expect(await component.getText()).toBe('Hello_World');
  });
  test('TextInputs can rewrite characters: Replace Space with No Character', async () => {
    const component = await app.findElementByTestID('rewrite_no_sp_input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('rewrite_no_sp_input');
    expect(dump).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'HelloWorld';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    expect(await component.getText()).toBe('HelloWorld');
  });
  test('TextInputs can rewrite characters: Replace Space with Clear', async () => {
    const component = await app.findElementByTestID('rewrite_clear_input');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('rewrite_clear_input');
    expect(dump).toMatchSnapshot();
    const clear = await app.findElementByTestID('rewrite_clear_button');
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'HelloWorld';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    expect(await component.getText()).toBe('HelloWorld');
    await clear.click();
    await app.waitUntil(
      async () => {
        return (await component.getText()) === '';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Clicking clear button should clear text.`,
      },
    );
    expect(await component.getText()).toBe('');
  });
  test('TextInputs can autocapitalize: Autocapitalize Turned Off', async () => {
    const component = await app.findElementByTestID('capitalize-none');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-none');
    expect(dump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await component.setValue('hello world');
        return (await component.getText()) === 'hello world';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    expect(await component.getText()).toBe('hello world');
  });
  // Comment out once the sentences mode has been implemented.
  /*test('TextInputs can autocapitalize: Autocapitalize Sentences', async () => {
    const component = await app.findElementByTestID('capitalize-sentences');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-sentences');
    expect(dump).toMatchSnapshot();

    // Test behavior when text is set from JS.
    // These TextInputs are currently empty. Setting defaultValue prop for them in TextInputSharedExamples.js
    // leads to an "override error". This file is expected to be a exact copy of its RN Core parent.
    expect(await component.getText()).toBe('initial text is not capitalized');

    await app.waitUntil(
      async () => {
        await component.setValue('hey here is a sentence. one more sentence? yeah one more sentence! and a last one.');
        return (await component.getText()) === 'Hey here is a sentence. One more sentence? Yeah one more sentence! And a last one.';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      });
  });*/
  // Comment out once the words mode has been implemented.
  /*test('TextInputs can autocapitalize: Autocapitalize Words', async () => {
    const component = await app.findElementByTestID('capitalize-words');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-words');
    expect(dump).toMatchSnapshot();

    // Test behavior when text is set from JS.
    // These TextInputs are currently empty. Setting defaultValue prop for them in TextInputSharedExamples.js
    // leads to an "override error". This file is expected to be a exact copy of its RN Core parent.
    expect(await component.getText()).toBe('initial text is not capitalized');
    
    await app.waitUntil(
      async () => {
        await component.setValue('hi i am autocapitalizing all words.');
        return (await component.getText()) === 'Hi I Am Autocapitalizing All Words.';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      });
  });*/
  test('TextInputs can autocapitalize: Autocapitalize Characters', async () => {
    const component = await app.findElementByTestID('capitalize-characters');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('capitalize-characters');
    expect(dump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await component.setValue(
          'hi i am setting up this whole UPPERCASE sentence.',
        );
        return (
          (await component.getText()) ===
          'HI I AM SETTING UP THIS WHOLE UPPERCASE SENTENCE.'
        );
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
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
    // Behavior not implemented yet
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
    // Behavior not implemented yet
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
    // Behavior not implemented yet
  });
  test('TextInputs can have customized line height', async () => {
    const component = await app.findElementByTestID('style-lineHeight');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-lineHeight');
    expect(dump).toMatchSnapshot();
    // Behavior not implemented yet
  });
  test('TextInputs can have text decoration lines', async () => {
    const component = await app.findElementByTestID('style-textDecorationLine');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-textDecorationLine');
    expect(dump).toMatchSnapshot();
    // Behavior not implemented yet
  });
  test('TextInputs can have text shadows', async () => {
    const component = await app.findElementByTestID('style-textShadow');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('style-textShadow');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can be set to not editable', async () => {
    const component = await app.findElementByTestID('textinput-not-editable');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-not-editable');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can be editable', async () => {
    const component = await app.findElementByTestID('textinput-editable');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-editable');
    expect(dump).toMatchSnapshot();
    await component.setValue('Hello World');
    expect(await component.getText()).toBe('Hello World');
  });
  test('TextInputs can be set to not editable 2', async () => {
    const component = await app.findElementByTestID('textinput-not-editable2');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-not-editable2');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can set their readOnly prop to false', async () => {
    const component = await app.findElementByTestID('textinput-readonly-false');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-readonly-false');
    expect(dump).toMatchSnapshot();
    await component.setValue('Hello World');
    expect(await component.getText()).toBe('Hello World');
  });
  test('TextInputs can set their readOnly prop to true', async () => {
    const component = await app.findElementByTestID('textinput-readyonly');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-readyonly');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can be defined as a set using accessibilityPosInSet and accessibilitySetSize', async () => {
    const component = await app.findElementByTestID('textinput-set');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-set');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can clear on submit', async () => {
    const component = await app.findElementByTestID(
      'textinput-clear-on-submit',
    );
    await component.waitForDisplayed({timeout: 5000});
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'Hello World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    await app.waitUntil(
      async () => {
        await component.setValue('\uE007');
        return (await component.getText()) === '';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    const dump = await dumpVisualTree('textinput-clear-on-submit');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can clear on submit with custom submit key event (Shift + Enter), single-line', async () => {
    const component = await app.findElementByTestID(
      'textinput-clear-on-submit-2',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-clear-on-submit-2');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can clear on submit with custom submit key event (Shift + Enter), multi-line', async () => {
    const component = await app.findElementByTestID(
      'textinput-clear-on-submit-3',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-clear-on-submit-3');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can submit with custom key, multilined and submit with enter', async () => {
    const component = await app.findElementByTestID(
      'textinput-clear-on-submit-4',
    );
    await component.waitForDisplayed({timeout: 5000});
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'Hello World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    await app.waitUntil(
      async () => {
        await component.setValue('\uE007');
        return (await component.getText()) === '';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    const dump = await dumpVisualTree('textinput-clear-on-submit-4');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can keep text on focus', async () => {
    const componentFocusFalse = await app.findElementByTestID(
      'clear-text-on-focus-false',
    );

    await componentFocusFalse.waitForDisplayed({timeout: 5000});
    await app.waitUntil(
      async () => {
        await componentFocusFalse.setValue('Hello World');
        return (await componentFocusFalse.getText()) === 'Hello World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );

    // Click on another textInput to move focus away from the one tested (clearTextOnFocus={false})
    const componentFocusTrue = await app.findElementByTestID(
      'clear-text-on-focus-true',
    );
    await componentFocusTrue.waitForDisplayed({timeout: 5000});
    await componentFocusTrue.click();

    // Then click on the tested component
    await componentFocusFalse.click();

    // Verify the textInput contents are still the same after regaining focus
    expect(await componentFocusFalse.getText()).toBe('Hello World');
  });
  test('TextInputs can clear text on focus', async () => {
    const componentFocusTrue = await app.findElementByTestID(
      'clear-text-on-focus-true',
    );

    await componentFocusTrue.waitForDisplayed({timeout: 5000});
    await app.waitUntil(
      async () => {
        await componentFocusTrue.setValue('Hello World');
        return (await componentFocusTrue.getText()) === 'Hello World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );

    // Click on another textInput to move focus away from the one tested (clearTextOnFocus={true})
    const componentFocusFalse = await app.findElementByTestID(
      'clear-text-on-focus-false',
    );
    await componentFocusFalse.waitForDisplayed({timeout: 5000});
    await componentFocusFalse.click();

    // Then click on the tested component
    await componentFocusTrue.click();

    // Verify the textInput contents are cleared after regaining focus
    expect(await componentFocusTrue.getText()).toBe('');
  });
  test('TextInputs can have inline images', async () => {
    const component = await app.findElementByTestID('textinput-inline-images');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-inline-images');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have inline images, drawableLeft and drawablePadding set', async () => {
    const component = await app.findElementByTestID(
      'textinput-inline-images-2',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-inline-images-2');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have inline images, drawable props not set', async () => {
    const component = await app.findElementByTestID(
      'textinput-inline-images-3',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-inline-images-3');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, none', async () => {
    const component = await app.findElementByTestID('textinput-return-none');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-none');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, go', async () => {
    const component = await app.findElementByTestID('textinput-return-go');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-go');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, search', async () => {
    const component = await app.findElementByTestID('textinput-return-search');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-search');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, send', async () => {
    const component = await app.findElementByTestID('textinput-return-send');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-send');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, done', async () => {
    const component = await app.findElementByTestID('textinput-return-done');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-done');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, previous', async () => {
    const component = await app.findElementByTestID(
      'textinput-return-previous',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-previous');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key type, next', async () => {
    const component = await app.findElementByTestID('textinput-return-next');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-next');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key label, Compile', async () => {
    const component = await app.findElementByTestID('textinput-return-Compile');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-Compile');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have custom return key label, React Native', async () => {
    const component = await app.findElementByTestID(
      'textinput-return-React Native',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-return-React Native');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocomplete, country', async () => {
    const component = await app.findElementByTestID(
      'textinput-autocomplete-country',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-autocomplete-country');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocomplete, address country', async () => {
    const component = await app.findElementByTestID(
      'textinput-autocomplete-address-country',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-autocomplete-address-country');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocomplete, one-time-code', async () => {
    const component = await app.findElementByTestID(
      'textinput-autocomplete-one-time-code',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-autocomplete-one-time-code');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autocomplete, sms-otp', async () => {
    const component = await app.findElementByTestID(
      'textinput-autocomplete-sms-otp',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-autocomplete-sms-otp');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can be multiline, topleft alignment', async () => {
    const component = await app.findElementByTestID(
      'textinput-multiline-topleft',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-multiline-topleft');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can be multiline, center alignment', async () => {
    const component = await app.findElementByTestID(
      'textinput-multiline-center',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-multiline-center');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can be multiline, bottomright alignment', async () => {
    const component = await app.findElementByTestID(
      'textinput-multiline-bottomright',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-multiline-bottomright');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs support secure entry', async () => {
    const component = await app.findElementByTestID('textinput-password');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-password');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs support secure entry, with placeholder text', async () => {
    const component = await app.findElementByTestID(
      'textinput-password-placeholder',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-password-placeholder');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customer letter spacing, spacing=0', async () => {
    const component = await app.findElementByTestID(
      'textinput-letterspacing-0',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-letterspacing-0');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customer letter spacing, spacing=2', async () => {
    const component = await app.findElementByTestID(
      'textinput-letterspacing-2',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-letterspacing-2');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customer letter spacing, spacing=9', async () => {
    const component = await app.findElementByTestID(
      'textinput-letterspacing-9',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-letterspacing-9');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customer letter spacing, spacing=-1', async () => {
    const component = await app.findElementByTestID(
      'textinput-letterspacing--1',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-letterspacing--1');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs with set height and padding from theme', async () => {
    const component = await app.findElementByTestID('textinput-theme-padding');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-theme-padding');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, default', async () => {
    const component = await app.findElementByTestID('textinput-weight-default');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-weight');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, normal', async () => {
    const component = await app.findElementByTestID('textinput-weight-normal');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-normal');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, bold', async () => {
    const component = await app.findElementByTestID('textinput-weight-bold');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-bold');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 900', async () => {
    const component = await app.findElementByTestID('textinput-weight-900');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-900');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 800', async () => {
    const component = await app.findElementByTestID('textinput-weight-800');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-800');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 700', async () => {
    const component = await app.findElementByTestID('textinput-weight-700');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-700');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 600', async () => {
    const component = await app.findElementByTestID('textinput-weight-600');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-600');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 500', async () => {
    const component = await app.findElementByTestID('textinput-weight-500');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-500');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 400', async () => {
    const component = await app.findElementByTestID('textinput-weight-400');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-400');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 300', async () => {
    const component = await app.findElementByTestID('textinput-weight-300');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-300');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 200', async () => {
    const component = await app.findElementByTestID('textinput-weight-200');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-200');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have customized font weight, 100', async () => {
    const component = await app.findElementByTestID('textinput-weight-100');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-100');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a default text color', async () => {
    const component = await app.findElementByTestID('textinput-default-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a custom text color', async () => {
    const component = await app.findElementByTestID('textinput-custom-color');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-custom-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a default placeholder text color', async () => {
    const component = await app.findElementByTestID(
      'textinput-default-placeholder-color',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-placeholder-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a custom placeholder text color', async () => {
    const component = await app.findElementByTestID(
      'textinput-custom-placeholder-color',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-custom-placeholder-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a default underline color', async () => {
    const component = await app.findElementByTestID(
      'textinput-default-underline-color',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-default-underline-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a custom underline color', async () => {
    const component = await app.findElementByTestID(
      'textinput-custom-underline-color',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-custom-underline-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a custom background color', async () => {
    const component = await app.findElementByTestID(
      'textinput-custom-background-color',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-custom-background-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs have a custom highlight color', async () => {
    const component = await app.findElementByTestID(
      'textinput-custom-highlight-color',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-custom-highlight-color');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can enable spellcheck', async () => {
    const component = await app.findElementByTestID('textinput-spellcheck');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-spellcheck');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can propagate events', async () => {
    const component = await app.findElementByTestID('textinput-propagation');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-propagation');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can register press events', async () => {
    const component = await app.findElementByTestID('textinput-press');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-press');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can autogrow', async () => {
    const component = await app.findElementByTestID('textinput-autogrow');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-autogrow');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can customize its padding', async () => {
    const component = await app.findElementByTestID('textinput-padding');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-padding');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have shadows', async () => {
    const component = await app.findElementByTestID('textinput-shadow');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-shadow');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs can have caretHidden', async () => {
    const component = await app.findElementByTestID('textinput-carethidden');
    await component.waitForDisplayed({timeout: 5000});
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'Hello World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    const dump = await dumpVisualTree('textinput-carethidden');
    expect(dump).toMatchSnapshot();
  });
  test('Text have cursorColor', async () => {
    const component = await app.findElementByTestID('textinput-cursorColor');
    await component.waitForDisplayed({timeout: 5000});
    await app.waitUntil(
      async () => {
        await component.setValue('Hello World');
        return (await component.getText()) === 'Hello World';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    const dump = await dumpVisualTree('textinput-cursorColor');
    expect(dump).toMatchSnapshot();
  });
  test('TextInputs which have a searchbox role should also support the value pattern.', async () => {
    const component = await app.findElementByTestID('textinput-searchbox');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('textinput-searchbox');
    expect(dump).toMatchSnapshot();
  });
});
