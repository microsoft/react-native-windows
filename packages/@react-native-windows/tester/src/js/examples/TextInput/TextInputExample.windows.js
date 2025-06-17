/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import type {
  RNTesterModule,
  RNTesterModuleExample,
} from '../../types/RNTesterTypes';

import ExampleTextInput from './ExampleTextInput';
import TextInputSharedExamples from './TextInputSharedExamples';
import React, {useState} from 'react';
import {StyleSheet, Switch, Text, View} from 'react-native';

const ToggleDefaultPaddingExample = (): React.Node => {
  const [hasPadding, setHasPadding] = React.useState(false);

  return (
    <View>
      <ExampleTextInput
        style={hasPadding ? {padding: 0} : null}
        testID="textinput-padding"
      />
      <Text onPress={() => setHasPadding(!hasPadding)}>Toggle padding</Text>
    </View>
  );
};

function AutogrowingTextInputExample({
  style,
  ...props
}: React.ElementConfig<typeof TextInput>) {
  const [multiline, setMultiline] = React.useState(true);
  const [fullWidth, setFullWidth] = React.useState(true);
  const [text, setText] = React.useState('');
  const [contentSize, setContentSize] = React.useState({width: 0, height: 0});

  return (
    <View>
      <Text>Full width:</Text>
      <Switch value={fullWidth} onValueChange={setFullWidth} />

      <Text>Multiline:</Text>
      <Switch value={multiline} onValueChange={setMultiline} />

      <Text>TextInput:</Text>
      <ExampleTextInput
        multiline={multiline}
        style={[style, {width: fullWidth ? '100%' : '50%'}]}
        onChangeText={setText}
        onContentSizeChange={({nativeEvent}) => {
          setContentSize({
            width: nativeEvent.contentSize.width,
            height: nativeEvent.contentSize.height,
          });
        }}
        {...props}
        testID={'textinput-autogrow'}
      />
      <Text>Plain text value representation:</Text>
      <Text>{text}</Text>
      <Text>Content Size: {JSON.stringify(contentSize)}</Text>
    </View>
  );
}

class PressInOutEvents extends React.Component<
  $FlowFixMeProps,
  $FlowFixMeState,
> {
  constructor(props) {
     super(props);
    this.state = {
      OnpressInText: 'OnPressIn message',
      OnpressOutText: 'OnPressOut message',
    };
  }

  render() {
    return (
      <View>
        <Text testID="textinput-state-display-in">{this.state.OnpressInText}</Text>
        <Text testID="textinput-state-display-out">{this.state.OnpressOutText}</Text>
        <ExampleTextInput
          placeholder="OnPressIn test."
          style={[styles.singleLineWithHeightTextInput]}
          onPressIn={() => {
            this.setState({OnpressInText: 'Holding down the click/touch'});
          }}
          testID="textinput-pressin"
        />
        <ExampleTextInput
          placeholder="OnPressOut test."
          style={[styles.singleLineWithHeightTextInput]}
          onPressOut={() => {
            this.setState({OnpressOutText: 'Released click/touch'});
          }}
          testID="textinput-pressout"
        />
      </View>
    );
  }
}

function PropagationSample() {
  const [eventLog, setEventLog] = useState([]);

  function logEvent(eventName) {
    const limit = 6;
    setEventLog(current => {
      return [eventName].concat(current.slice(0, limit - 1));
    });
    console.log(eventName);
  }
  return (
    <>
      <View
        focusable
        style={styles.row}
        keyDownEvents={[
          {code: 'KeyW', handledEventPhase: 3},
          {code: 'KeyE', handledEventPhase: 1},
        ]}
        onKeyDown={event => logEvent('outer keyDown ' + event.nativeEvent.code)}
        onKeyDownCapture={event =>
          logEvent('outer keyDownCapture ' + event.nativeEvent.code)
        }>
        <Text>some text to focus on</Text>
        <ExampleTextInput
          placeholder="Click inside the box to observe events being fired."
          style={[styles.singleLineWithHeightTextInput]}
          onKeyDown={event =>
            logEvent('textinput keyDown ' + event.nativeEvent.code)
          }
          onKeyUp={event =>
            logEvent('textinput keyUp ' + event.nativeEvent.code)
          }
          keyDownEvents={[
            {code: 'KeyW', handledEventPhase: 3},
            {code: 'KeyE', handledEventPhase: 1},
          ]}
          testID="textinput-propagation"
        />
      </View>
      <View style={styles.eventLogBox}>
        {eventLog.map((e, ii) => (
          <Text key={ii}>{e}</Text>
        ))}
      </View>
    </>
  );
}

function SpellCheckSample() {
  const [spellCheckEnabled, setSpellCheckEnabled] = useState(true);
  return (
    <>
      <Text>Spell Check Enabled:</Text>
      <Switch
        /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was
         * found when making Flow check .android.js files. */
        value={spellCheckEnabled}
        /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was
         * found when making Flow check .android.js files. */
        onValueChange={setSpellCheckEnabled}
      />
      <ExampleTextInput
        placeholder="Type text to test spell check functionality."
        style={[styles.singleLineWithHeightTextInput]}
        spellCheck={spellCheckEnabled}
        testID="textinput-spellcheck"
      />
    </>
  );
}

const styles = StyleSheet.create({
  multiline: {
    height: 60,
    fontSize: 16,
  },
  singleLine: {
    fontSize: 16,
  },
  singleLineWithHeightTextInput: {
    height: 30,
  },
  wrappedText: {
    maxWidth: 300,
  },
});

const examples: Array<RNTesterModuleExample> = [
  ...TextInputSharedExamples.filter(example => example.name !== 'clipping'), // [Windows #13303 - Text must be in <Text> Control]
  {
    title: 'Clear text on focus',
    render: function (): React.Node {
      return (
        <View>
          <Text>Do not clear text on focus:</Text>
          <ExampleTextInput
            style={styles.singleLine}
            testID="clear-text-on-focus-false"
          />
          <Text>Clear text on focus:</Text>
          <ExampleTextInput
            clearTextOnFocus={true}
            style={styles.singleLine}
            testID="clear-text-on-focus-true"
          />
        </View>
      );
    },
  },
  {
    title: 'Select text on focus',
    render: function (): React.Node {
      return (
        <View>
          <Text>Select text on focus:</Text>
          <ExampleTextInput
            selectTextOnFocus={true}
            style={styles.singleLine}
            testID="select-text-on-focus"
          />
          <Text>
            Do not select text on focus if clear text on focus is enabled:
          </Text>
          <ExampleTextInput
            selectTextOnFocus={true}
            clearTextOnFocus={true}
            style={styles.singleLine}
            testID="select-text-on-focus-while-clear-text-on-focus"
          />
        </View>
      );
    },
  },
  {
    title: 'Colors and text inputs',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            style={[styles.singleLine]}
            defaultValue="Default color text"
            testID="textinput-default-color"
          />
          <ExampleTextInput
            style={[styles.singleLine, {color: 'green'}]}
            defaultValue="Green Text"
            testID="textinput-custom-color"
          />
          <ExampleTextInput
            placeholder="Default placeholder text color"
            style={styles.singleLine}
            testID="textinput-default-placeholder-color"
          />
          <ExampleTextInput
            placeholder="Red placeholder text color"
            placeholderTextColor="red"
            style={styles.singleLine}
            testID="textinput-custom-placeholder-color"
          />
          <ExampleTextInput
            placeholder="Default underline color"
            style={styles.singleLine}
            testID="textinput-default-underline-color"
          />
          <ExampleTextInput
            placeholder="Blue underline color"
            style={styles.singleLine}
            underlineColorAndroid="blue"
            testID="textinput-custom-underline-color"
          />
          <ExampleTextInput
            defaultValue="Same BackgroundColor as View "
            style={[
              styles.singleLine,
              {backgroundColor: 'rgba(100, 100, 100, 0.3)'},
            ]}
            testID="textinput-custom-background-color">
            <Text style={{backgroundColor: 'rgba(100, 100, 100, 0.3)'}}>
              Darker backgroundColor
            </Text>
          </ExampleTextInput>
          <ExampleTextInput
            defaultValue="Selection Color is red"
            selectionColor={'red'}
            style={styles.singleLine}
            testID="textinput-custom-highlight-color"
          />
          <ExampleTextInput
            defaultValue="Selection handles are red"
            selectionHandleColor={'red'}
            style={styles.singleLine}
          />
          <ExampleTextInput
            defaultValue="Cursor Color is red"
            cursorColor={'red'}
            style={styles.singleLine}
          />
        </View>
      );
    },
  },
  {
    title: 'Font Weight',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            defaultValue="Font Weight (default)"
            style={[styles.singleLine]}
            testID="textinput-weight-default"
          />
          {[
            'normal',
            'bold',
            '900',
            800,
            '700',
            '600',
            '500',
            '400',
            '300',
            '200',
            '100',
          ].map(fontWeight => (
            <ExampleTextInput
              defaultValue={`Font Weight (${fontWeight})`}
              key={fontWeight}
              style={[styles.singleLine, {fontWeight}]}
              testID={'textinput-weight-' + fontWeight}
            />
          ))}
        </View>
      );
    },
  },
  {
    title: 'Font Family',
    render: function (): React.Node {
      return (
        <View testID="style-fontFamily">
          <ExampleTextInput
            defaultValue="Font Family (default)"
            style={[styles.singleLine]}
            testID="textinput-family-default"
          />
          {[
            'normal',
            'Times New Roman',
            'Courier New',
            'Arial',
            'Comic Sans MS',
            'Georgia',
            'Verdana',
          ].map(fontFamily => (
            <ExampleTextInput
              defaultValue={`Font Family (${fontFamily})`}
              key={fontFamily}
              style={[styles.singleLine, {fontFamily}]}
              testID={'textinput-family-' + fontFamily}
            />
          ))}
        </View>
      );
    },
  },
  {
    title: 'Text input, themes and heights',
    render: function (): React.Node {
      return (
        <ExampleTextInput
          placeholder="If you set height, beware of padding set from themes"
          style={[styles.singleLineWithHeightTextInput]}
          testID="textinput-theme-padding"
        />
      );
    },
  },
  {
    title: 'letterSpacing',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            style={[styles.singleLine, {letterSpacing: 0}]}
            placeholder="letterSpacing = 0"
            testID="textinput-letterspacing-0"
          />
          <ExampleTextInput
            style={[styles.singleLine, {letterSpacing: 2}]}
            placeholder="letterSpacing = 2"
            testID="textinput-letterspacing-2"
          />
          <ExampleTextInput
            style={[styles.singleLine, {letterSpacing: 9}]}
            placeholder="letterSpacing = 9"
            testID="textinput-letterspacing-9"
          />
          <ExampleTextInput
            style={[styles.singleLine, {letterSpacing: -1}]}
            placeholder="letterSpacing = -1"
            testID="textinput-letterspacing--1"
          />
        </View>
      );
    },
  },
  {
    title: 'Passwords',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            defaultValue="iloveturtles"
            secureTextEntry={true}
            style={styles.singleLine}
            testID="textinput-password"
          />
          <ExampleTextInput
            secureTextEntry={true}
            style={[styles.singleLine, {color: 'red'}]}
            placeholder="color is supported too"
            placeholderTextColor="red"
            testID="textinput-password-placeholder"
          />
        </View>
      );
    },
  },
  {
    title: 'Editable',
    render: function (): React.Node {
      return (
        <ExampleTextInput
          defaultValue="Can't touch this! (>'-')> ^(' - ')^ <('-'<) (>'-')> ^(' - ')^"
          editable={false}
          style={styles.singleLine}
          testID="textinput-not-editable"
        />
      );
    },
  },
  {
    title: 'Multiline',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            autoCorrect={true}
            placeholder="multiline, aligned top-left"
            placeholderTextColor="red"
            multiline={true}
            style={[
              styles.multiline,
              {textAlign: 'left', textAlignVertical: 'top'},
            ]}
            testID="textinput-multiline-topleft"
          />
          <ExampleTextInput
            autoCorrect={true}
            placeholder="multiline, aligned center"
            placeholderTextColor="green"
            multiline={true}
            style={[
              styles.multiline,
              {textAlign: 'center', textAlignVertical: 'center'},
            ]}
            testID="textinput-multiline-center"
          />
          <ExampleTextInput
            autoCorrect={true}
            multiline={true}
            style={[
              styles.multiline,
              {color: 'blue'},
              {textAlign: 'right', textAlignVertical: 'bottom'},
            ]}
            testID="textinput-multiline-bottomright">
            <Text style={styles.multiline}>
              multiline with children, aligned bottom-right
            </Text>
          </ExampleTextInput>
        </View>
      );
    },
  },
  {
    title: 'Editable and Read only',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            placeholder="editable text input using editable prop"
            editable
            testID="textinput-editable"
          />
          <ExampleTextInput
            placeholder="uneditable text input using editable prop"
            editable={false}
            testID="textinput-not-editable2"
          />
          <ExampleTextInput
            placeholder="editable text input using readOnly prop"
            readOnly={false}
            testID="textinput-readonly-false"
          />
          <ExampleTextInput
            placeholder="uneditable text input using readOnly prop"
            readOnly
            testID="textinput-readyonly"
          />
        </View>
      );
    },
  },
  {
    title: 'Fixed number of lines',
    platform: 'android',
    render: function (): React.Node {
      return (
        <View style={styles.wrappedText}>
          <ExampleTextInput
            numberOfLines={2}
            multiline={true}
            placeholder="Two line input using numberOfLines prop"
          />
          <ExampleTextInput
            numberOfLines={5}
            multiline={true}
            placeholder="Five line input using numberOfLines prop"
          />
          <ExampleTextInput
            rows={2}
            multiline={true}
            placeholder="Two line input using rows prop"
          />
          <ExampleTextInput
            rows={5}
            multiline={true}
            placeholder="Five line input using rows prop"
          />
        </View>
      );
    },
  },
  {
    title: 'allowFontScaling attribute',
    render: function (): React.Node {
      return (
        <View>
          <Text>
            By default, text will respect Text Size accessibility setting on
            Android. It means that all font sizes will be increased or decreased
            depending on the value of the Text Size setting in the OS's Settings
            app.
          </Text>
          <ExampleTextInput
            placeholder="allowFontScaling = false"
            allowFontScaling={false}
          />
          <ExampleTextInput
            style={{marginTop: 10}}
            placeholder="allowFontScaling = false"
            allowFontScaling={true}
          />
        </View>
      );
    },
  },
  {
    title: 'maxFontSizeMultiplier attribute',
    name: 'maxFontSizeMultiplier',
    render(): React.Node {
      return (
        <View testID={'max-font-size-multiplier'}>
          <Text>
            When allowFontScaling is enabled, you can use the
            maxFontSizeMultiplier prop to set an upper limit on how much the
            font size will be scaled.
          </Text>
          <ExampleTextInput
            allowFontScaling={true}
            maxFontSizeMultiplier={1}
            placeholder="This text will not scale up (max 1x)"
            style={{marginTop: 10}}
            testID={'non-scalable-text-input'}
          />
          <ExampleTextInput
            allowFontScaling={true}
            maxFontSizeMultiplier={1.5}
            placeholder="This text will scale up (max 1.5x)"
            style={{marginTop: 10}}
            testID={'scalable-text-input'}
          />
        </View>
      );
    },
  },
  {
    title: 'Auto-expanding',
    render: function (): React.Node {
      return (
        <View style={styles.wrappedText}>
          <AutogrowingTextInputExample
            enablesReturnKeyAutomatically={true}
            returnKeyType="done"
            multiline={true}
            style={{maxHeight: 400, minHeight: 20, backgroundColor: '#eeeeee'}}>
            <Text style={{fontSize: 6, color: 'red'}}>
              small small small small small small
            </Text>
            <Text>regular regular</Text>
            <Text style={{fontSize: 30, color: 'green'}}>
              huge huge huge huge huge
            </Text>
          </AutogrowingTextInputExample>
        </View>
      );
    },
  },
  {
    title: 'Text Auto Complete',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            autoComplete="country"
            placeholder="country"
            style={styles.default}
            testID="textinput-autocomplete-country"
          />
          <ExampleTextInput
            autoComplete="postal-address-country"
            placeholder="postal-address-country"
            style={styles.default}
            testID="textinput-autocomplete-address-country"
          />
          <ExampleTextInput
            autoComplete="one-time-code"
            placeholder="one-time-code"
            style={styles.default}
            testID="textinput-autocomplete-one-time-code"
          />
          <ExampleTextInput
            autoComplete="sms-otp"
            placeholder="sms-otp"
            style={styles.default}
            testID="textinput-autocomplete-sms-otp"
          />
          <ExampleTextInput autoComplete="sms-otp" placeholder="sms-otp" />
        </View>
      );
    },
  },
  {
    title: 'Return key',
    render: function (): React.Node {
      const returnKeyTypes = [
        'none',
        'go',
        'search',
        'send',
        'done',
        'previous',
        'next',
      ];
      const returnKeyLabels = ['Compile', 'React Native'];
      const returnKeyExamples = returnKeyTypes.map(type => {
        return (
          <ExampleTextInput
            key={type}
            returnKeyType={type}
            placeholder={'returnKeyType: ' + type}
            style={styles.singleLine}
            testID={'textinput-return-' + type}
          />
        );
      });
      const types = returnKeyLabels.map(type => {
        return (
          <ExampleTextInput
            key={type}
            returnKeyLabel={type}
            placeholder={'returnKeyLabel: ' + type}
            style={styles.singleLine}
            testID={'textinput-return-' + type}
          />
        );
      });
      return (
        <View>
          {returnKeyExamples}
          {types}
        </View>
      );
    },
  },
  {
    title: 'Inline Images',
    render: function (): React.Node {
      return (
        <View>
          <ExampleTextInput
            inlineImageLeft="ic_menu_black_24dp"
            placeholder="This has drawableLeft set"
            style={styles.singleLine}
            testID="textinput-inline-images"
          />
          <ExampleTextInput
            inlineImageLeft="ic_menu_black_24dp"
            inlineImagePadding={30}
            placeholder="This has drawableLeft and drawablePadding set"
            style={styles.singleLine}
            testID="textinput-inline-images-2"
          />
          <ExampleTextInput
            placeholder="This does not have drawable props set"
            style={styles.singleLine}
            testID="textinput-inline-images-3"
          />
        </View>
      );
    },
  },
  {
    title: 'Toggle Default Padding',
    render: function (): React.Node {
      return <ToggleDefaultPaddingExample />;
    },
  },
  {
    title: 'onPressIn, onPressOut events',
    render: function (): React.Node {
      return <PressInOutEvents />;
    },
  },
  // [Windows
  {
    title: 'Clear text on submit',
    render: function (): React.Node {
      return (
        <View>
          <Text>Default submit key (Enter):</Text>
          <ExampleTextInput
            clearTextOnSubmit
            style={styles.singleLine}
            testID="textinput-clear-on-submit"
          />
          <Text>Custom submit key event (Shift + Enter), single-line:</Text>
          <ExampleTextInput
            clearTextOnSubmit
            style={styles.singleLine}
            submitKeyEvents={[{code: 'Enter', shiftKey: true}]}
            testID="textinput-clear-on-submit-2"
          />
          <Text>Custom submit key event (Shift + Enter), multi-line:</Text>
          <ExampleTextInput
            multiline
            clearTextOnSubmit
            style={styles.multiline}
            submitKeyEvents={[{code: 'Enter', shiftKey: true}]}
            testID="textinput-clear-on-submit-3"
          />
          <Text>Submit with Enter key, return key with Shift + Enter</Text>
          <ExampleTextInput
            multiline
            clearTextOnSubmit
            style={styles.multiline}
            submitKeyEvents={[{code: 'Enter'}]}
            testID="textinput-clear-on-submit-4"
          />
        </View>
      );
    },
  },
  {
    title: 'Stop propagation sample',
    render: function (): React.Node {
      return <PropagationSample />;
    },
  },
  {
    title: 'Toggle spell check',
    render: function (): React.Node {
      return <SpellCheckSample />;
    },
  },
  {
    title: 'CaretHidden set to True',
    render: function (): React.Node {
      return (
        <View>
          <Text>CaretHidden</Text>
          <ExampleTextInput
            style={styles.singleLine}
            caretHidden={true}
            placeholder="caretHidden={true}"
            testID="textinput-carethidden"
          />
        </View>
      );
    },
  },
  {
    title: 'Cursorcolor set to Green',
    render: function (): React.Node {
      return (
        <View>
          <Text>Cursorcolor</Text>
          <ExampleTextInput
            style={styles.singleLine}
            cursorColor={'green'}
            placeholder='cursorColor={"green"}'
            testID="textinput-cursorColor"
          />
        </View>
      );
    },
  },
  {
    title: 'Shadow set to Purple',
    render: function (): React.Node {
      return (
        <View>
          <Text>Shadow</Text>
          <ExampleTextInput
            placeholder="shadowColor: purple"
            testID="textinput-shadow"
            style={{
              fontSize: 16,
              shadowColor: 'purple',
              shadowOpacity: 99,
              shadowOffset: {width: 10, height: 10},
              shadowRadius: 20,
            }}
          />
        </View>
      );
    },
  },
  {
    title: 'Set of TextInputs',
    render: function (): React.Node {
      return (
        <View accessible testID="textinput-set">
          <ExampleTextInput
            style={styles.singleLine}
            placeholder="TextInput 1 in Set of 3"
            accessibilityPosInSet={1}
            accessibilitySetSize={3}
            accessibilityLiveRegion="none"
          />
          <ExampleTextInput
            style={styles.singleLine}
            placeholder="TextInput 2 in Set of 3"
            accessibilityPosInSet={2}
            accessibilitySetSize={3}
            accessibilityLiveRegion="polite"
          />
          <ExampleTextInput
            style={styles.singleLine}
            placeholder="TextInput 3 in Set of 3"
            accessibilityPosInSet={3}
            accessibilitySetSize={3}
            accessibilityLiveRegion="assertive"
          />
        </View>
      );
    },
  },
  {
    title: 'Searchbox',
    render: function (): React.Node {
      return (
        <View accessible testID="textinput-searchbox">
          <ExampleTextInput
            style={styles.singleLine}
            accessibilityRole="searchbox"
            defaultValue="Search"
          />
        </View>
      );
    },
  },
  // Windows]
];

module.exports = ({
  displayName: (undefined: ?string),
  title: 'TextInput',
  documentationURL: 'https://reactnative.dev/docs/textinput',
  category: 'Basic',
  description: 'Single and multi-line text inputs.',
  examples,
}: RNTesterModule);
