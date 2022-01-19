/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 * @generate-docs
 */

'use strict';

import * as React from 'react';
import Platform from '../Utilities/Platform';
import StyleSheet, {type ColorValue} from '../StyleSheet/StyleSheet';
import Text from '../Text/Text';
// [Windows
// import TouchableNativeFeedback from './Touchable/TouchableNativeFeedback';
// import TouchableOpacity from './Touchable/TouchableOpacity';
import TouchableHighlight from './Touchable/TouchableHighlight';
import {PlatformColor} from '../StyleSheet/PlatformColorValueTypes';
// Windows]
import View from './View/View';
import invariant from 'invariant';

import type {
  AccessibilityState,
  AccessibilityActionEvent,
  AccessibilityActionInfo,
} from './View/ViewAccessibility';
import type {PressEvent} from '../Types/CoreEventTypes';

type ButtonProps = $ReadOnly<{|
  /**
    Text to display inside the button. On Android the given title will be
    converted to the uppercased form.
   */
  title: string,

  /**
    Handler to be called when the user taps the button. The first function
    argument is an event in form of [PressEvent](pressevent).
   */
  onPress: (event?: PressEvent) => mixed,

  /**
    If `true`, doesn't play system sound on touch.

    @platform android

    @default false
   */
  touchSoundDisabled?: ?boolean,

  /**
    Color of the text (iOS), or background color of the button (Android).

    @default {@platform android} '#2196F3'
    @default {@platform ios} '#007AFF'
   */
  color?: ?ColorValue,

  /**
    TV preferred focus.

    @platform tv

    @default false
   */
  hasTVPreferredFocus?: ?boolean,

  /**
    Designates the next view to receive focus when the user navigates down. See
    the [Android documentation][android:nextFocusDown].

    [android:nextFocusDown]:
    https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusDown

    @platform android, tv
   */
  nextFocusDown?: ?number,

  /**
    Designates the next view to receive focus when the user navigates forward.
    See the [Android documentation][android:nextFocusForward].

    [android:nextFocusForward]:
    https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusForward

    @platform android, tv
   */
  nextFocusForward?: ?number,

  /**
    Designates the next view to receive focus when the user navigates left. See
    the [Android documentation][android:nextFocusLeft].

    [android:nextFocusLeft]:
    https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusLeft

    @platform android, tv
   */
  nextFocusLeft?: ?number,

  /**
    Designates the next view to receive focus when the user navigates right. See
    the [Android documentation][android:nextFocusRight].

    [android:nextFocusRight]:
    https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusRight

    @platform android, tv
   */
  nextFocusRight?: ?number,

  /**
    Designates the next view to receive focus when the user navigates up. See
    the [Android documentation][android:nextFocusUp].

    [android:nextFocusUp]:
    https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusUp

    @platform android, tv
   */
  nextFocusUp?: ?number,

  /**
    Text to display for blindness accessibility features.
   */
  accessibilityLabel?: ?string,

  /**
    If `true`, disable all interactions for this component.

    @default false
   */
  disabled?: ?boolean,

  /**
    Used to locate this view in end-to-end tests.
   */
  testID?: ?string,

  /**
   * Accessibility props.
   */
  accessible?: ?boolean,
  accessibilityActions?: ?$ReadOnlyArray<AccessibilityActionInfo>,
  onAccessibilityAction?: ?(event: AccessibilityActionEvent) => mixed,
  accessibilityState?: ?AccessibilityState,
  accessibilityHint?: ?string,

  // [Windows
  /**
    Set the order in which elements receive focus when the user navigates through them by pressing Tab.
   */
  tabIndex?: ?number,
  // Windows]
|}>;

/**
  A basic button component that should render nicely on any platform. Supports a
  minimal level of customization.

  If this button doesn't look right for your app, you can build your own button
  using [TouchableOpacity](touchableopacity) or
  [TouchableWithoutFeedback](touchablewithoutfeedback). For inspiration, look at
  the [source code for this button component][button:source]. Or, take a look at
  the [wide variety of button components built by the community]
  [button:examples].

  [button:source]:
  https://github.com/facebook/react-native/blob/HEAD/Libraries/Components/Button.js

  [button:examples]:
  https://js.coach/?menu%5Bcollections%5D=React%20Native&page=1&query=button

  ```jsx
  <Button
    onPress={onPressLearnMore}
    title="Learn More"
    color="#841584"
    accessibilityLabel="Learn more about this purple button"
  />
  ```

  ```SnackPlayer name=Button%20Example
  import React from 'react';
  import { StyleSheet, Button, View, SafeAreaView, Text, Alert } from 'react-native';

  const Separator = () => (
    <View style={styles.separator} />
  );

  const App = () => (
    <SafeAreaView style={styles.container}>
      <View>
        <Text style={styles.title}>
          The title and onPress handler are required. It is recommended to set accessibilityLabel to help make your app usable by everyone.
        </Text>
        <Button
          title="Press me"
          onPress={() => Alert.alert('Simple Button pressed')}
        />
      </View>
      <Separator />
      <View>
        <Text style={styles.title}>
          Adjust the color in a way that looks standard on each platform. On  iOS, the color prop controls the color of the text. On Android, the color adjusts the background color of the button.
        </Text>
        <Button
          title="Press me"
          color="#f194ff"
          onPress={() => Alert.alert('Button with adjusted color pressed')}
        />
      </View>
      <Separator />
      <View>
        <Text style={styles.title}>
          All interaction for the component are disabled.
        </Text>
        <Button
          title="Press me"
          disabled
          onPress={() => Alert.alert('Cannot press this one')}
        />
      </View>
      <Separator />
      <View>
        <Text style={styles.title}>
          This layout strategy lets the title define the width of the button.
        </Text>
        <View style={styles.fixToText}>
          <Button
            title="Left button"
            onPress={() => Alert.alert('Left button pressed')}
          />
          <Button
            title="Right button"
            onPress={() => Alert.alert('Right button pressed')}
          />
        </View>
      </View>
    </SafeAreaView>
  );

  const styles = StyleSheet.create({
    container: {
      flex: 1,
      justifyContent: 'center',
      marginHorizontal: 16,
    },
    title: {
      textAlign: 'center',
      marginVertical: 8,
    },
    fixToText: {
      flexDirection: 'row',
      justifyContent: 'space-between',
    },
    separator: {
      marginVertical: 8,
      borderBottomColor: '#737373',
      borderBottomWidth: StyleSheet.hairlineWidth,
    },
  });

  export default App;
  ```
 */

class Button extends React.Component<
  ButtonProps,
  {hover: boolean, pressed: boolean},
> {
  // [Windows
  constructor(props: Object) {
    super(props);
    this.state = {
      hover: false,
      pressed: false,
    };
  }
  // Windows]
  render(): React.Node {
    const {
      accessibilityLabel,
      color,
      onPress,
      touchSoundDisabled,
      title,
      hasTVPreferredFocus,
      nextFocusDown,
      nextFocusForward,
      nextFocusLeft,
      nextFocusRight,
      nextFocusUp,
      testID,
      accessible,
      accessibilityActions,
      accessibilityHint,
      onAccessibilityAction,
      tabIndex,
    } = this.props;
    const buttonStyles = [styles.button];
    const textStyles = [styles.text];
    if (color) {
      if (Platform.OS === 'ios') {
        textStyles.push({color: color});
      } else {
        buttonStyles.push({backgroundColor: color});
      }
    }

    const disabled =
      this.props.disabled != null
        ? this.props.disabled
        : this.props.accessibilityState?.disabled;

    const accessibilityState =
      disabled !== this.props.accessibilityState?.disabled
        ? {...this.props.accessibilityState, disabled}
        : this.props.accessibilityState;

    if (disabled) {
      buttonStyles.push(styles.buttonDisabled);
      textStyles.push(styles.textDisabled);
    }

    invariant(
      typeof title === 'string',
      'The title prop of a Button must be a string',
    );
    const formattedTitle =
      Platform.OS === 'android' ? title.toUpperCase() : title;
    // [Windows - render a TouchableHighlight
    const Touchable = TouchableHighlight;
    //  Platform.OS === 'android' ? TouchableNativeFeedback : TouchableOpacity;
    // Windows]
    // [Windows
    if (Platform.OS === 'windows') {
      return (
        <Touchable
          accessibilityLabel={accessibilityLabel}
          accessibilityHint={accessibilityHint}
          accessibilityRole="button"
          accessibilityState={accessibilityState}
          hasTVPreferredFocus={hasTVPreferredFocus}
          nextFocusDown={nextFocusDown}
          nextFocusForward={nextFocusForward}
          nextFocusLeft={nextFocusLeft}
          nextFocusRight={nextFocusRight}
          nextFocusUp={nextFocusUp}
          testID={testID}
          disabled={disabled}
          onPress={onPress}
          tabIndex={tabIndex}
          touchSoundDisabled={touchSoundDisabled}
          underlayColor={
            color
              ? PlatformColor('SolidBackgroundFillColorBaseBrush')
              : PlatformColor('ButtonBackgroundPressed')
          }
          onShowUnderlay={() => {
            this.setState({pressed: true});
          }}
          onHideUnderlay={() => {
            this.setState({pressed: false});
          }}
          style={
            color
              ? {borderRadius: 3}
              : this.state.pressed
              ? [buttonStyles, styles.buttonPressed]
              : this.state.hover
              ? [buttonStyles, styles.buttonHover]
              : buttonStyles
          }
          onMouseEnter={() => {
            if (!disabled) this.setState({hover: true});
          }}
          onMouseLeave={() => {
            if (!disabled) this.setState({hover: false});
          }}>
          <View
            style={
              color
                ? this.state.pressed
                  ? [buttonStyles, styles.buttonPressed]
                  : buttonStyles
                : {}
            }>
            <View
              style={
                color
                  ? this.state.hover
                    ? [
                        styles.buttonHover,
                        {
                          backgroundColor: 'rgba(255, 255, 255, 0.1)',
                        },
                      ]
                    : {}
                  : {}
              }>
              <Text
                style={
                  color
                    ? textStyles
                    : this.state.pressed
                    ? [
                        textStyles,
                        {
                          color: PlatformColor('ButtonForegroundPressed'),
                        },
                      ]
                    : this.state.hover
                    ? [
                        textStyles,
                        {
                          color: PlatformColor('ButtonForegroundPointerOver'),
                        },
                      ]
                    : textStyles
                }
                disabled={disabled}>
                {formattedTitle}
              </Text>
            </View>
          </View>
        </Touchable>
      );
    } else {
      return (
        <Touchable
          accessible={accessible}
          accessibilityActions={accessibilityActions}
          onAccessibilityAction={onAccessibilityAction}
          accessibilityLabel={accessibilityLabel}
          accessibilityHint={accessibilityHint}
          accessibilityRole="button"
          accessibilityState={accessibilityState}
          hasTVPreferredFocus={hasTVPreferredFocus}
          nextFocusDown={nextFocusDown}
          nextFocusForward={nextFocusForward}
          nextFocusLeft={nextFocusLeft}
          nextFocusRight={nextFocusRight}
          nextFocusUp={nextFocusUp}
          testID={testID}
          disabled={disabled}
          onPress={onPress}
          touchSoundDisabled={touchSoundDisabled}>
          <View style={buttonStyles}>
            <Text style={textStyles} disabled={disabled}>
              {formattedTitle}
            </Text>
          </View>
        </Touchable>
      );
    }
    // Windows]
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
    // [Windows
    windows: {
      backgroundColor: PlatformColor('ButtonBackground'),
      borderRadius: 3,
      borderColor: PlatformColor('ButtonBorderBrush'),
      borderWidth: 1,
      borderBottomWidth: 1.5,
    },
    // Windows]
  }),
  text: {
    textAlign: 'center',
    margin: 8,
    ...Platform.select({
      ios: {
        // iOS blue from https://developer.apple.com/ios/human-interface-guidelines/visual-design/color/
        color: '#007AFF',
        fontSize: 18,
      },
      android: {
        color: 'white',
        fontWeight: '500',
      },
      // [Windows
      windows: {
        color: PlatformColor('ButtonForeground'),
        fontWeight: '400',
        fontSize: 14,
      },
      // Windows]
    }),
  },
  buttonDisabled: Platform.select({
    ios: {},
    android: {
      elevation: 0,
      backgroundColor: '#dfdfdf',
    },
    windows: {
      backgroundColor: PlatformColor('ButtonBackgroundDisabled'),
      borderColor: PlatformColor('ButtonBorderBrushDisabled'),
    },
  }),
  textDisabled: Platform.select({
    ios: {
      color: '#cdcdcd',
    },
    android: {
      color: '#a1a1a1',
    },
    // [Windows
    windows: {
      color: PlatformColor('ButtonForegroundDisabled'),
    },
    // Windows]
  }),
  // [Windows
  buttonHover: {
    backgroundColor: PlatformColor('ButtonBackgroundPointerOver'),
    borderColor: PlatformColor('ButtonBorderBrushPointerOver'),
    borderRadius: 3,
  },
  buttonPressed: {
    borderColor: PlatformColor('ButtonBorderBrushPressed'),
    borderBottomWidth: 1,
  },
  // Windows]
});

module.exports = Button;
