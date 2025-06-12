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

const {RNTesterThemeContext} = require('../../components/RNTesterTheme');
const React = require('react');
const {Alert, Button, StyleSheet, View} = require('react-native');

function onButtonPress(buttonName: string) {
  Alert.alert(`Your application has been ${buttonName}!`);
}

exports.displayName = 'ButtonExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Button';
exports.documentationURL = 'https://reactnative.dev/docs/button';
exports.description = 'Simple React Native button component.';

exports.examples = [
  {
    title: 'Button with default styling',
    render: function (): React.Node {
      return (
        <Button
          onPress={() => onButtonPress('submitted')}
          testID="button_default_styling"
          title="Submit Application"
          accessibilityLabel="Press to submit your application!"
        />
      );
    },
  },
  {
    title: 'Button with color="red"',
    description:
      ('Note: On iOS, the color prop controls the color of the text. On ' +
        'Android, the color adjusts the background color of the button.': string),
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <Button
                onPress={() => onButtonPress('cancelled')}
                testID="cancel_button"
                color={theme.SystemRedColor}
                title="Cancel Application"
                accessibilityLabel="Press to cancel your application!"
              />
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Two Buttons with Flexbox layout',
    description:
      ('Two buttons wrapped inside view with justifyContent: spaceBetween,' +
        'This layout strategy lets the title define the width of the button': string),
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <View
                style={styles.container}
                testID="two_button_container"
                accessible>
                <Button
                  onPress={() => onButtonPress('cancelled')}
                  testID="two_cancel_button"
                  color={theme.SystemRedColor}
                  title="Cancel"
                  accessibilityLabel="Press to cancel your application!"
                />
                <Button
                  onPress={() => onButtonPress('submitted')}
                  testID="two_submit_button"
                  color={theme.SystemGreenColor}
                  title="Submit"
                  accessibilityLabel="Press to submit your application!"
                />
              </View>
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Three Buttons with Flexbox layout',
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <View
                style={styles.container}
                testID="three_button_container"
                accessible>
                <Button
                  onPress={() => onButtonPress('cancelled')}
                  testID="three_cancel_button"
                  color={theme.SystemRedColor}
                  title="Cancel"
                  accessibilityLabel="Press to cancel your application!"
                />
                <Button
                  onPress={() => onButtonPress('saved')}
                  testID="three_save_button"
                  color={theme.LinkColor}
                  title="Save For Later"
                  accessibilityLabel="Press to save your application!"
                />
                <Button
                  onPress={() => onButtonPress('submitted')}
                  testID="three_submit_button"
                  color={theme.SystemGreenColor}
                  title="Submit"
                  accessibilityLabel="Press to submit your application!"
                />
              </View>
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Button with disabled={true}',
    description:
      'By passing disabled={true} all interactions for the button are disabled.',
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <Button
                disabled
                onPress={() => onButtonPress('submitted')}
                color={theme.LinkColor}
                testID="disabled_button"
                title="Submit Application"
                accessibilityLabel="Press to submit your application!"
              />
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Button with accessibilityLabel="label"',
    description: ('Note: This prop changes the text that a screen ' +
      'reader announces (there are no visual differences).': string),
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <Button
                onPress={() => onButtonPress('submitted')}
                testID="accessibilityLabel_button"
                color={theme.LinkColor}
                title="Submit Application"
                accessibilityLabel="Press to submit your application!"
              />
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Button with accessibilityState={{disabled: true}}',
    description:
      ('Note: This prop will announce on TalkBack that the button is disabled. ' +
        'The "disabled" prop has higher precedence on the state of the component': string),
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <Button
                accessibilityState={{disabled: true}}
                onPress={() => onButtonPress('submitted')}
                testID="accessibilityState_button"
                color={theme.LinkColor}
                title="Submit Application"
                accessibilityLabel="Press to submit your application!"
              />
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Button with Accessibility Props',
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <Button
                onPress={() => onButtonPress('submitted')}
                testID="accessibility_props"
                title="Submit Application"
                accessibilityLabel="Press to submit your application!"
                accessibilityRole="button"
                accessibilityHint="Submit Button"
                accessibilityPosInSet={1}
                accessibilitySetSize={1}
                accessibilityLiveRegion="assertive"
                accessibilityValue={{Text: 'Submit Application'}}
              />
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Button with Accessible and Focusable Prop',
    render: function (): React.Node {
      return (
        <RNTesterThemeContext.Consumer>
          {theme => {
            return (
              <View testID="accessible_focusable_button">
                <Button
                  onPress={() => onButtonPress('submitted')}
                  testID="default_button"
                  title="Default Button"
                  accessibilityLabel="Press to submit your application!"
                />
                <Button
                  onPress={() => onButtonPress('submitted')}
                  testID="accessible_false_button"
                  title="Button with accessible=false"
                  accessibilityLabel="Press to submit your application!"
                  accessible={false}
                />
                <Button
                  onPress={() => onButtonPress('submitted')}
                  testID="focusable_false_button"
                  title="Button with focusable=false"
                  accessibilityLabel="Press to submit your application!"
                  focusable={false}
                />
                <Button
                  onPress={() => onButtonPress('submitted')}
                  testID="accessible_focusable_false_button"
                  title="Button with accessible=false and focusable=false"
                  accessibilityLabel="Press to submit your application!"
                  accessible={false}
                  focusable={false}
                />
              </View>
            );
          }}
        </RNTesterThemeContext.Consumer>
      );
    },
  },
  {
    title: 'Button with dynamic text',
    description: 'Button text updates when pressed',
    render: function (): React.Node {
      return <DynamicTextButton />;
    },
  },
  {
    title: 'Button with dynamic color',
    description: 'Button color updates when pressed',
    render: function (): React.Node {
      return <DynamicColorButton />;
    },
  },
  {
    title: 'Button with dynamic disabled state',
    description: 'Button disabled state toggles when pressed',
    render: function (): React.Node {
      return <DynamicDisabledButton />;
    },
  },
  {
    title: 'Button with dynamic styling on press',
    description: 'Button updates styling when pressed',
    render: function (): React.Node {
      return <DynamicStyleButton />;
    },
  },
];

// Dynamic Button Components for fast refresh testing
function DynamicTextButton(): React.Node {
  const [buttonText, setButtonText] = React.useState('Initial Text');
  const [pressCount, setPressCount] = React.useState(0);

  const onPress = () => {
    const newCount = pressCount + 1;
    setPressCount(newCount);
    setButtonText(`Pressed ${newCount} times`);
  };

  return (
    <Button
      onPress={onPress}
      testID="dynamic_text_button"
      title={buttonText}
      accessibilityLabel="Press to change button text"
    />
  );
}

function DynamicColorButton(): React.Node {
  const [colorIndex, setColorIndex] = React.useState(0);
  const colors = ['#007AFF', '#FF3B30', '#34C759', '#FF9500', '#5856D6'];
  
  const onPress = () => {
    setColorIndex((prev) => (prev + 1) % colors.length);
  };

  return (
    <RNTesterThemeContext.Consumer>
      {theme => (
        <Button
          onPress={onPress}
          testID="dynamic_color_button"
          color={colors[colorIndex]}
          title="Change Color"
          accessibilityLabel="Press to change button color"
        />
      )}
    </RNTesterThemeContext.Consumer>
  );
}

function DynamicDisabledButton(): React.Node {
  const [isDisabled, setIsDisabled] = React.useState(false);
  const [toggleText, setToggleText] = React.useState('Disable Me');

  const onPress = () => {
    if (!isDisabled) {
      setIsDisabled(true);
      setToggleText('Disabled');
      // Re-enable after 2 seconds for testing
      setTimeout(() => {
        setIsDisabled(false);
        setToggleText('Disable Me');
      }, 2000);
    }
  };

  return (
    <Button
      disabled={isDisabled}
      onPress={onPress}
      testID="dynamic_disabled_button"
      title={toggleText}
      accessibilityLabel="Press to toggle disabled state"
    />
  );
}

function DynamicStyleButton(): React.Node {
  const [isPressed, setIsPressed] = React.useState(false);
  const [pressCount, setPressCount] = React.useState(0);

  const onPress = () => {
    setIsPressed(true);
    setPressCount(prev => prev + 1);
    // Reset pressed state after visual feedback
    setTimeout(() => setIsPressed(false), 300);
  };

  return (
    <RNTesterThemeContext.Consumer>
      {theme => (
        <View style={[styles.dynamicContainer, isPressed && styles.pressedContainer]} testID="dynamic_style_container">
          <Button
            onPress={onPress}
            testID="dynamic_style_button"
            color={isPressed ? theme.SystemRedColor : theme.LinkColor}
            title={`Style Button (${pressCount})`}
            accessibilityLabel="Press to update styling"
          />
        </View>
      )}
    </RNTesterThemeContext.Consumer>
  );
}

const styles = StyleSheet.create({
  container: {
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
  dynamicContainer: {
    padding: 10,
    borderRadius: 5,
    backgroundColor: 'transparent',
  },
  pressedContainer: {
    backgroundColor: '#f0f0f0',
  },
});
