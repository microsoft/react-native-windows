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
];

const styles = StyleSheet.create({
  container: {
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
});
