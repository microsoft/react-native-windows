/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule Button
 * @flow
 */
'use strict';

const ColorPropType = require('ColorPropType');
const Platform = require('Platform');
const React = require('React');
const PropTypes = require('prop-types');
const StyleSheet = require('StyleSheet');
const Text = require('Text');
const TouchableNativeFeedback = require('TouchableNativeFeedback');
const TouchableOpacity = require('TouchableOpacity');
const View = require('View');
const createFocusableComponent = require('FocusableWindows');

const invariant = require('fbjs/lib/invariant');

const FocusableView = createFocusableComponent(View);

const KEY_CODE_ENTER = FocusableView.keys.Enter;
const KEY_CODE_SPACE = FocusableView.keys.Space;

const DOWN_KEYCODES = [KEY_CODE_SPACE, KEY_CODE_ENTER];
const UP_KEYCODES = [KEY_CODE_SPACE];


/**
 * A basic button component that should render nicely on any platform. Supports
 * a minimal level of customization.
 *
 * <center><img src="img/buttonExample.png"></img></center>
 *
 * If this button doesn't look right for your app, you can build your own
 * button using [TouchableOpacity](docs/touchableopacity.html)
 * or [TouchableNativeFeedback](docs/touchablenativefeedback.html).
 * For inspiration, look at the [source code for this button component](https://github.com/facebook/react-native/blob/master/Libraries/Components/Button.js).
 * Or, take a look at the [wide variety of button components built by the community](https://js.coach/react-native?search=button).
 *
 * Example usage:
 *
 * ```
 * <Button
 *   onPress={onPressLearnMore}
 *   title="Learn More"
 *   color="#841584"
 *   accessibilityLabel="Learn more about this purple button"
 * />
 * ```
 *
 */

class Button extends React.Component {

  props: {
    title: string,
    onPress: () => any,
    color?: ?string,
    accessibilityLabel?: ?string,
    disabled?: ?boolean,
    testID?: ?string,
  };

  static propTypes = {
    /**
     * Text to display inside the button
     */
    title: PropTypes.string.isRequired,
    /**
     * Text to display for blindness accessibility features
     */
    accessibilityLabel: PropTypes.string,
    /**
     * Color of the text (iOS, Windows), or background color of the button (Android)
     */
    color: ColorPropType,
    /**
     * If true, disable all interactions for this component.
     */
    disabled: PropTypes.bool,
    /**
     * Handler to be called when the user taps the button
     */
    onPress: PropTypes.func.isRequired,
    /**
     * Used to locate this view in end-to-end tests.
     */
    testID: PropTypes.string,
    /**
     * tabIndex:
     * -1: Control is not keyboard focusable in any way
     * 0 (default): Control is keyboard focusable in the normal order
     * >0: Control is keyboard focusable in a priority order (starting with 1)
     *
     * @platform windows
     */
    tabIndex: PropTypes.number,
    /**
     * Controls whether control should use system default provided focus rects
     * @platform windows
     */
    disableSystemFocusVisuals: PropTypes.bool,
    /**
     * Callback that is called when the text input is blurred
     * @platform windows
     */
    onBlur: PropTypes.func,
    /**
     * Callback that is called when the text input is focused
     * @platform windows
     */
    onFocus: PropTypes.func,
};

  render() {
    const {
      accessibilityLabel,
      color,
      onPress,
      title,
      disabled,
      testID,
    } = this.props;
    const buttonStyles = [styles.button];
    const textStyles = [styles.text];
    if (color) {
      if (Platform.OS === 'ios' || Platform.OS === 'windows') {
        textStyles.push({color: color});
      } else {
        buttonStyles.push({backgroundColor: color});
      }
    }
    const accessibilityTraits = ['button'];
    if (disabled) {
      buttonStyles.push(styles.buttonDisabled);
      textStyles.push(styles.textDisabled);
      accessibilityTraits.push('disabled');
    }
    invariant(
      typeof title === 'string',
      'The title prop of a Button must be a string',
    );
    const formattedTitle = Platform.OS === 'android' ? title.toUpperCase() : title;
    const Touchable = Platform.OS === 'android' ? TouchableNativeFeedback : TouchableOpacity;
    let content;
    if (Platform.OS === "windows") {
      const tabIndex = this.props.tabIndex || 0;
      const windowsTabFocusable = !disabled && tabIndex >= 0;
      content =
        <FocusableView
          ref={this._setFocusableRef}
          disabled={disabled}
          isTabStop={windowsTabFocusable}
          tabIndex={tabIndex}
          disableSystemFocusVisuals={this.props.disableSystemFocusVisuals}
          handledKeyDownKeys={DOWN_KEYCODES}
          handledKeyUpKeys={UP_KEYCODES}
          onKeyDown={this._onKeyDown}
          onKeyUp={this._onKeyUp}
          onFocus={this._onFocus}
          onBlur={this._onBlur}
          style={buttonStyles}
        >
          <Text style={textStyles} disabled={disabled}>{formattedTitle}</Text>
      </FocusableView>;
    } else {
      content =
        <View style={buttonStyles}>
          <Text style={textStyles} disabled={disabled}>{formattedTitle}</Text>
        </View>;      
    }

    return (
      <Touchable
        accessibilityComponentType="button"
        accessibilityLabel={accessibilityLabel}
        accessibilityTraits={accessibilityTraits}
        testID={testID}
        disabled={disabled}
        onPress={onPress}>
          {content}
      </Touchable>
    );
  }

  _setFocusableRef = (ref): void => {
    this._focusableRef = ref;
  }

  _onKeyDown = (e): void => {
    if (!this.props.disabled) {
      if (this.props.onPress) {
        const key = e.nativeEvent.keyCode;
        // ENTER triggers press on key down
        if (key === KEY_CODE_ENTER) {
          this.props.onPress(e);
        }
      }
    }
  }

  _onKeyUp = (e): void => {
    if (!this.props.disabled) {
      if (this.props.onPress) {
        const key = e.nativeEvent.keyCode;
        // SPACE triggers press on key up
        if (key === KEY_CODE_SPACE) {
          this.props.onPress(e);
        }
      }
    }
  }

  _onFocus = (e): void => {
    if (this.props.onFocus) {
      this.props.onFocus(e);
    }
  }

  _onBlur = (e): void => {
    if (this.props.onBlur) {
      this.props.onBlur(e);
    }
  }

  focus() {
    if (!this.props.disabled &&
        this._focusableRef &&
        this._focusableRef.focus) {
          this._focusableRef.focus();
    }
  }

  blur() {
    if (this._focusableRef &&
        this._focusableRef.blur) {
        this._focusableRef.blur();
    }
  }
}

const styles = StyleSheet.create({
  button: Platform.select({
    ios: {},
    android: {
      elevation: 4,
      // Material design blue from https://material.google.com/style/color.html#color-color-palette
      backgroundColor: '#2196F3',
      borderRadius: 2,
    },
    windows: {
      backgroundColor: 'rgba(0, 0, 0, 0.2)',
      paddingLeft: 8,
      paddingTop: 4,
      paddingRight: 8,
      paddingBottom: 4,
    }
  }),
  text: Platform.select({
    ios: {
      // iOS blue from https://developer.apple.com/ios/human-interface-guidelines/visual-design/color/
      color: '#007AFF',
      textAlign: 'center',
      padding: 8,
      fontSize: 18,
    },
    android: {
      color: 'white',
      textAlign: 'center',
      padding: 8,
      fontWeight: '500',
    },
    windows: {
      color: 'black',
      textAlign: 'center',
      fontSize: 15,
    }
  }),
  buttonDisabled: Platform.select({
    ios: {},
    android: {
      elevation: 0,
      backgroundColor: '#dfdfdf',
    },
    windows: {}
  }),
  textDisabled: Platform.select({
    ios: {
      color: '#cdcdcd',
    },
    android: {
      color: '#a1a1a1',
    },
    windows: {
      color: 'rgba(0, 0, 0, 0.4)',
    }
  }),
});

module.exports = Button;
