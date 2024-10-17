/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

import type {RNTesterModule} from '../../types/RNTesterTypes';

import * as React from 'react';
import {
  Alert,
  Animated,
  Image,
  Platform,
  Pressable,
  StyleSheet,
  Text,
  TouchableHighlight,
  View,
  Switch,
} from 'react-native';
import ReactNativeFeatureFlags from 'react-native/Libraries/ReactNative/ReactNativeFeatureFlags';

const {useEffect, useRef, useState} = React;

function onPressablePress(pressableName: string) {
  Alert.alert(`Your application has been ${pressableName}!`);
}

const forceTouchAvailable =
  (Platform.OS === 'ios' && Platform.constants.forceTouchAvailable) || false;

function ContentPress() {
  const [timesPressed, setTimesPressed] = useState(0);

  let textLog = '';
  if (timesPressed > 1) {
    textLog = timesPressed + 'x onPress';
  } else if (timesPressed > 0) {
    textLog = 'onPress';
  }

  return (
    <>
      <View style={styles.row}>
        <Pressable
          onPress={() => {
            setTimesPressed(current => current + 1);
          }}>
          {({pressed}) => (
            <Text testID="one_press_me_button" style={styles.text}>
              {pressed ? 'Pressed!' : 'Press Me'}
            </Text>
          )}
        </Pressable>
      </View>
      <View style={styles.logBox}>
        <Text testID="pressable_press_console">{textLog}</Text>
      </View>
    </>
  );
}

function TextOnPressBox() {
  const [timesPressed, setTimesPressed] = useState(0);

  let textLog = '';
  if (timesPressed > 1) {
    textLog = timesPressed + 'x text onPress';
  } else if (timesPressed > 0) {
    textLog = 'text onPress';
  }

  return (
    <>
      <Text
        style={styles.textBlock}
        testID="tappable_text"
        onPress={() => {
          setTimesPressed(prev => prev + 1);
        }}>
        Text has built-in onPress handling
      </Text>
      <View style={styles.logBox}>
        <Text testID="tappable_text_console">{textLog}</Text>
      </View>
    </>
  );
}

function PressableAriaLabel() {
  return (
    <View style={[styles.row, styles.centered]}>
      <Pressable
        style={styles.wrapper}
        testID="pressable_aria_label"
        aria-label="pressable with aria label"
        accessibilityRole="button"
        onPress={() => onPressablePress('pressed')}>
        <Text style={styles.button}>Press Me</Text>
      </Pressable>
    </View>
  );
}
function PressableFeedbackEvents() {
  const [eventLog, setEventLog] = useState<Array<string>>([]);

  function appendEvent(eventName: string) {
    const limit = 6;
    setEventLog(current => {
      return [eventName].concat(current.slice(0, limit - 1));
    });
  }

  return (
    <View testID="pressable_feedback_events">
      <View style={[styles.row, styles.centered]}>
        <Pressable
          style={styles.wrapper}
          testID="pressable_feedback_events_button"
          accessibilityLabel="pressable feedback events"
          accessibilityRole="button"
          accessibilityValue={{text: 'Press Me'}}
          // [Windows
          onFocus={() => appendEvent('focus')}
          onBlur={() => appendEvent('blur')}
          onHoverIn={() => appendEvent('hover in')}
          onHoverOut={() => appendEvent('hover out')}
          // Windows]
          onPress={() => appendEvent('press')}
          onPressIn={() => appendEvent('pressIn')}
          onPressOut={() => appendEvent('pressOut')}
          onLongPress={() => appendEvent('longPress')}>
          <Text style={styles.button}>Press Me</Text>
        </Pressable>
      </View>
      <View
        testID="pressable_feedback_events_console"
        accessible
        style={styles.eventLogBox}>
        {eventLog.map((e, ii) => (
          <Text key={ii}>{e}</Text>
        ))}
      </View>
    </View>
  );
}

function PressableDelayEvents() {
  const [eventLog, setEventLog] = useState<Array<string>>([]);

  function appendEvent(eventName: string) {
    const limit = 6;
    const newEventLog = eventLog.slice(0, limit - 1);
    newEventLog.unshift(eventName);
    setEventLog(newEventLog);
  }

  return (
    <View testID="pressable_delay_events">
      <View style={[styles.row, styles.centered]}>
        <Pressable
          style={styles.wrapper}
          testID="pressable_delay_events_button"
          onPress={() => appendEvent('press')}
          onPressIn={() => appendEvent('pressIn')}
          onPressOut={() => appendEvent('pressOut')}
          delayLongPress={800}
          onLongPress={() => appendEvent('longPress - 800ms delay')}>
          <Text style={styles.button}>Press Me</Text>
        </Pressable>
      </View>
      <View
        style={styles.eventLogBox}
        testID="pressable_delay_events_console"
        accessible>
        {eventLog.map((e, ii) => (
          <Text key={ii}>{e}</Text>
        ))}
      </View>
    </View>
  );
}

function ForceTouchExample() {
  const [force, setForce] = useState(0);

  const consoleText = forceTouchAvailable
    ? 'Force: ' + force.toFixed(3)
    : '3D Touch is not available on this device';

  return (
    <View testID="pressable_3dtouch_event">
      <View style={styles.forceTouchBox} testID="pressable_3dtouch_output">
        <Text>{consoleText}</Text>
      </View>
      <View style={[styles.row, styles.centered]}>
        <View
          style={styles.wrapper}
          testID="pressable_3dtouch_button"
          onStartShouldSetResponder={() => true}
          // $FlowFixMe[sketchy-null-number]
          onResponderMove={event => setForce(event.nativeEvent?.force || 1)}
          onResponderRelease={event => setForce(0)}>
          <Text style={styles.button}>Press Me</Text>
        </View>
      </View>
    </View>
  );
}

function PressableHitSlop() {
  const [timesPressed, setTimesPressed] = useState(0);

  let log = '';
  if (timesPressed > 1) {
    log = timesPressed + 'x onPress';
  } else if (timesPressed > 0) {
    log = 'onPress';
  }

  return (
    <View testID="pressable_hit_slop">
      <View style={[styles.row, styles.centered]}>
        <Pressable
          onPress={() => setTimesPressed(num => num + 1)}
          style={styles.hitSlopWrapper}
          hitSlop={{top: 30, bottom: 30, left: 60, right: 60}}
          testID="pressable_hit_slop_button">
          <Text style={styles.hitSlopButton}>Press Outside This View</Text>
        </Pressable>
      </View>
      <View style={styles.logBox}>
        <Text>{log}</Text>
      </View>
    </View>
  );
}

function PressableNativeMethods() {
  const [status, setStatus] = useState<?boolean>(null);
  const ref = useRef<$FlowFixMe>(null);

  useEffect(() => {
    setStatus(ref.current != null && typeof ref.current.measure === 'function');
  }, []);

  return (
    <>
      <View style={[styles.row, styles.block]}>
        <Pressable ref={ref}>
          <View />
        </Pressable>
        <Text>
          {status == null
            ? 'Missing Ref!'
            : status === true
              ? 'Native Methods Exist'
              : 'Native Methods Missing!'}
        </Text>
      </View>
    </>
  );
}

function PressableDisabled() {
  return (
    <>
      <Pressable
        disabled={true}
        style={[styles.row, styles.block]}
        testID="pressable_disabled_true">
        <Text style={styles.disabledButton}>Disabled Pressable</Text>
      </Pressable>

      <Pressable
        disabled={false}
        testID="pressable_disabled_false"
        style={({pressed}) => [
          {opacity: pressed ? 0.5 : 1},
          styles.row,
          styles.block,
        ]}>
        <Text style={styles.button}>Enabled Pressable</Text>
      </Pressable>
    </>
  );
}

function PressableHoverStyle() {
  const [hovered, setHovered] = useState(false);
  return (
    <View style={styles.row}>
      <Pressable
        style={[
          {
            backgroundColor: hovered ? 'rgb(210, 230, 255)' : 'white',
          },
          styles.wrapperCustom,
        ]}
        onHoverIn={() => setHovered(true)}
        onHoverOut={() => setHovered(false)}
        testID="hover-style-pressable">
        <Text style={styles.text}>Hover Me</Text>
      </Pressable>
    </View>
  );
}

function PressableFocusCallbacks() {
  const [timesPressed, setTimesPressed] = useState(0);

  let textLog = '';
  if (timesPressed > 1) {
    textLog = timesPressed + 'x onPress';
  } else if (timesPressed > 0) {
    textLog = 'onPress';
  }
  const viewRef = useRef<React.ElementRef<typeof Pressable> | null>(null);

  const focusViewPressed = () => {
    viewRef.current.focus();
  };

  return (
    <>
      <View style={styles.row}>
        <Pressable
          ref={viewRef}
          onFocus={() => console.log('Pressable onFocus')}
          onBlur={() => console.log('Pressable onBlur')}
          onPress={() => {
            setTimesPressed(current => current + 1);
          }}>
          {({pressed}) => (
            <Text style={styles.text}>{pressed ? 'Pressed!' : 'Press Me'}</Text>
          )}
        </Pressable>
      </View>
      <View style={styles.logBox}>
        <Text testID="pressable_press_console">{textLog}</Text>
      </View>
      <TouchableHighlight
        style={{height: 30}}
        onPress={focusViewPressed}
        underlayColor={'transparent'}>
        <View>
          <Text>Click to focus textbox</Text>
        </View>
      </TouchableHighlight>
    </>
  );
}

function PressWithOnKeyDown() {
  const [timesPressed, setTimesPressed] = useState(0);
  const [text, setText] = useState('defaultText');

  let textLog = '';
  if (timesPressed > 1) {
    textLog = timesPressed + 'x onPress';
  } else if (timesPressed > 0) {
    textLog = 'onPress';
  }

  const [shouldPreventDefault, setShouldPreventDefault] = useState(false);
  const toggleSwitch = () =>
    setShouldPreventDefault(previousState => !previousState);

  function myKeyDown(event) {
    console.log('keyDown - ' + event.nativeEvent.code);
    setText(event.nativeEvent.code);
    if (shouldPreventDefault) {
      event.preventDefault();
    }
  }
  function myKeyUp(event) {
    console.log('keyUp - ' + event.nativeEvent.code);
    setText(event.nativeEvent.code);
    if (shouldPreventDefault) {
      event.preventDefault();
    }
  }

  return (
    <>
      <View style={styles.row}>
        <Pressable
          onKeyDown={event => myKeyDown(event)}
          onKeyUp={event => myKeyUp(event)}
          onPress={() => {
            setTimesPressed(current => current + 1);
          }}>
          {({pressed}) => (
            <Text style={styles.text}>{pressed ? 'Pressed!' : 'Press Me'}</Text>
          )}
        </Pressable>
        <Switch onValueChange={toggleSwitch} value={shouldPreventDefault} />
      </View>
      <View style={styles.logBox}>
        <Text testID="pressable_press_console">{textLog}</Text>
        <Text>{text}</Text>
      </View>
    </>
  );
}

function PressWithKeyCapture() {
  const [eventLog, setEventLog] = useState([]);
  const [timesPressed, setTimesPressed] = useState(0);

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
        style={styles.row}
        onKeyDown={event => logEvent('outer keyDown ' + event.nativeEvent.code)}
        onKeyDownCapture={event =>
          logEvent('outer keyDownCapture ' + event.nativeEvent.code)
        }>
        <Pressable
          keyDownEvents={[
            {code: 'KeyW', handledEventPhase: 3},
            {code: 'KeyE', handledEventPhase: 1},
          ]}
          onKeyDown={event => logEvent('keyDown ' + event.nativeEvent.code)}
          onKeyDownCapture={event =>
            logEvent('keyDownCapture ' + event.nativeEvent.code)
          }
          onPress={() => {
            setTimesPressed(current => current + 1);
            logEvent('pressed ' + timesPressed);
          }}>
          {({pressed}) => (
            <Text style={styles.text}>{pressed ? 'Pressed!' : 'Press Me'}</Text>
          )}
        </Pressable>
      </View>

      <View style={styles.eventLogBox}>
        {eventLog.map((e, ii) => (
          <Text key={ii}>{e}</Text>
        ))}
      </View>
    </>
  );
}

const styles = StyleSheet.create({
  row: {
    justifyContent: 'center',
    flexDirection: 'row',
  },
  centered: {
    justifyContent: 'center',
  },
  text: {
    fontSize: 16,
  },
  block: {
    padding: 10,
  },
  button: {
    color: '#007AFF',
  },
  disabledButton: {
    color: '#007AFF',
    opacity: 0.5,
  },
  hitSlopButton: {
    color: 'white',
  },
  wrapper: {
    borderRadius: 8,
  },
  wrapperCustom: {
    borderRadius: 8,
    padding: 6,
    width: '100%',
    justifyContent: 'center',
    alignItems: 'center',
  },
  hitSlopWrapper: {
    backgroundColor: 'red',
    marginVertical: 30,
  },
  logBox: {
    padding: 20,
    margin: 10,
    borderWidth: StyleSheet.hairlineWidth,
    borderColor: '#f0f0f0',
    backgroundColor: '#f9f9f9',
  },
  eventLogBox: {
    padding: 10,
    margin: 10,
    height: 120,
    borderWidth: StyleSheet.hairlineWidth,
    borderColor: '#f0f0f0',
    backgroundColor: '#f9f9f9',
  },
  forceTouchBox: {
    padding: 10,
    margin: 10,
    borderWidth: StyleSheet.hairlineWidth,
    borderColor: '#f0f0f0',
    backgroundColor: '#f9f9f9',
    alignItems: 'center',
  },
  textBlock: {
    fontWeight: '500',
    color: 'blue',
  },
  image: {
    height: 100,
    width: 100,
  },
});

const examples = [
  {
    title: 'Change content based on Press',
    render(): React.Node {
      return <ContentPress />;
    },
  },
  {
    title: 'Change style based on Press',
    render(): React.Node {
      return (
        <View style={styles.row}>
          <Pressable
            style={({pressed}) => [
              {
                backgroundColor: pressed ? 'rgb(210, 230, 255)' : 'white',
              },
              styles.wrapperCustom,
            ]}
            testID="style-change-pressable">
            <Text style={styles.text}>Press Me</Text>
          </Pressable>
        </View>
      );
    },
  },
  {
    title: 'Change child based on Press',
    description:
      ('You should be able to press the button, move your finger while pressing, and release it with the proper status updates.': string),
    render(): React.Node {
      return (
        <View style={styles.row}>
          <Pressable
            style={({pressed}) => [
              {
                backgroundColor: pressed ? 'rgb(210, 230, 255)' : 'white',
              },
              styles.wrapperCustom,
            ]}>
            {({pressed}) => (
              <>
                {pressed && <Text style={styles.text}>Pressed!</Text>}
                {!pressed && (
                  <Text style={styles.text}>Press me and move your finger</Text>
                )}
              </>
            )}
          </Pressable>
        </View>
      );
    },
  },
  {
    title: 'Pressable feedback events',
    description: ('<Pressable> components accept onPress, onPressIn, ' +
      'onPressOut, and onLongPress as props.': string),
    render: function (): React.Node {
      return <PressableFeedbackEvents />;
    },
  },
  {
    title: 'Pressable with Ripple and Animated child',
    description:
      ('Pressable can have an AnimatedComponent as a direct child.': string),
    platform: 'android',
    render: function (): React.Node {
      const mScale = new Animated.Value(1);
      Animated.timing(mScale, {
        toValue: 0.3,
        duration: 1000,
        useNativeDriver: false,
      }).start();
      const style = {
        backgroundColor: 'rgb(180, 64, 119)',
        width: 200,
        height: 100,
        transform: [{scale: mScale}],
      };
      return (
        <View style={styles.row}>
          <Pressable android_ripple={{color: 'green'}}>
            <Animated.View style={style} />
          </Pressable>
        </View>
      );
    },
  },
  {
    title: 'Pressable with custom Ripple',
    description:
      ("Pressable can specify ripple's radius, color and borderless params": string),
    platform: 'android',
    render: function (): React.Node {
      const nativeFeedbackButton = {
        textAlign: 'center',
        margin: 10,
      };
      return (
        <View>
          <View
            style={[
              styles.row,
              {justifyContent: 'space-around', alignItems: 'center'},
            ]}>
            <Pressable
              android_ripple={{color: 'orange', borderless: true, radius: 30}}>
              <View>
                <Text style={[styles.button, nativeFeedbackButton]}>
                  radius 30
                </Text>
              </View>
            </Pressable>

            <Pressable android_ripple={{borderless: true, radius: 150}}>
              <View>
                <Text style={[styles.button, nativeFeedbackButton]}>
                  radius 150
                </Text>
              </View>
            </Pressable>

            <Pressable android_ripple={{borderless: false, radius: 70}}>
              <View style={styles.block}>
                <Text style={[styles.button, nativeFeedbackButton]}>
                  radius 70, with border
                </Text>
              </View>
            </Pressable>
          </View>

          <Pressable android_ripple={{borderless: false}}>
            <View style={styles.block}>
              <Text style={[styles.button, nativeFeedbackButton]}>
                with border, default color and radius
              </Text>
            </View>
          </Pressable>

          <View style={{alignItems: 'center'}}>
            <Pressable
              android_ripple={{
                borderless: false,
                foreground: true,
              }}>
              <Image
                source={{
                  uri: 'https://www.facebook.com/ads/pics/successstories.png',
                }}
                style={styles.image}
              />
            </Pressable>
            <Text>use foreground</Text>
          </View>
        </View>
      );
    },
  },
  {
    title: '<Text onPress={fn}> with highlight',
    render: function (): React.Node {
      return <TextOnPressBox />;
    },
  },
  {
    title: 'Pressable delay for events',
    description: ('<Pressable> also accept delayPressIn, ' +
      'delayPressOut, and delayLongPress as props. These props impact the ' +
      'timing of feedback events.': string),
    render: function (): React.Node {
      return <PressableDelayEvents />;
    },
  },
  {
    title: '3D Touch / Force Touch',
    description:
      'iPhone 8 and 8 plus support 3D touch, which adds a force property to touches',
    render: function (): React.Node {
      return <ForceTouchExample />;
    },
    platform: 'ios',
  },
  {
    title: 'Pressable Hit Slop',
    description:
      ('<Pressable> components accept hitSlop prop which extends the touch area ' +
        'without changing the view bounds.': string),
    render: function (): React.Node {
      return <PressableHitSlop />;
    },
  },
  {
    title: 'Pressable Native Methods',
    description:
      ('<Pressable> components expose native methods like `measure`.': string),
    render: function (): React.Node {
      return <PressableNativeMethods />;
    },
  },
  {
    title: 'Disabled Pressable',
    description:
      ('<Pressable> components accept disabled prop which prevents ' +
        'any interaction with component': string),
    render: function (): React.Node {
      return <PressableDisabled />;
    },
  },
  {
    title: 'Pressable with aria-label="label"',
    description: ('Note: This prop changes the text that a screen ' +
      'reader announces (there are no visual differences).': string),
    render: function (): React.Node {
      return <PressableAriaLabel />;
    },
  },
  {
    title: 'Focusability in Pressable',
    description:
      ('<Pressable> components can be receive focus by calling the focus() and blur() methods on them.' +
        'They also expose onFocus and onBlur callbacks to hadle incoming native events.': string),
    render: function (): React.Node {
      return <PressableFocusCallbacks />;
    },
  },
  {
    title: 'OnKeyDown/OnKeyUp callbacks on Pressable',
    description:
      ('<Pressable> components can be respond to keyDown/keyUp native events.' +
        ' Additionally, they can be activated by pressing Space or Enter as if they were clicked with the mouse, triggering onPress' +
        ' - this behavior can be suppressed by calling e.preventDefault() on the event (can be toggled with the switch).': string),
    render: function (): React.Node {
      return <PressWithOnKeyDown />;
    },
  },
  {
    title: 'OnKeyDownCapture on Pressable (View)',
    description:
      ('You can intercept routed KeyDown/KeyUp events by specifying the onKeyDownCapture/onKeyUpCapture callbacks.' +
        " In the example below, a <Pressable> is wrapper in a <View>, and each specifies onKeyDown and onKeyDownCapture callbacks. Set focus to the 'Press me' element by tabbing into it, and start pressing letters on the keyboard to observe the event log below." +
        " Additionally, because the keyDownEvents prop is specified - keyDownEvents={[{code: 'KeyW', handledEventPhase: 3}, {code: 'KeyE', handledEventPhase: 1}]} - " +
        'for these keys the event routing will be interrupted (by a call to event.stopPropagation()) at the phase specified (3 - bubbling, 1 - capturing) to match processing on the native side.': string),
    render: function (): React.Node {
      return <PressWithKeyCapture />;
    },
  },
  {
    title: 'Pressable with Advanced Borders',
    description:
      'Pressables can have individual border width and radius per edge/corner.',
    render: function (): React.Node {
      return (
        <View>
          <Pressable
            testID="advanced_borders_pressable"
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'grey',
              borderColor: 'blue',
              borderBottomWidth: 5,
              borderLeftWidth: 10,
              borderRightWidth: 15,
              borderTopWidth: 20,
              borderBottomEndRadius: 1,
              borderTopEndRadius: 2,
              borderTopStartRadius: 3,
              borderBottomStartRadius: 4,
            }}
            onPress={() => {}}
          />
        </View>
      );
    },
  },
  {
    title: 'Pressable with Opacity',
    description: 'Pressables can have ranging opacity values.',
    render: function (): React.Node {
      return (
        <View testID="opacity_pressable" accessible>
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.1,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.2,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.3,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.4,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.5,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.6,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.7,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.8,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 0.9,
            }}
            onPress={() => {}}
          />
          <Pressable
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'black',
              opacity: 1,
            }}
            onPress={() => {}}
          />
        </View>
      );
    },
  },
  {
    title: 'Pressable with Keyboard and Accessibility Focus',
    description:
      'Pressables can have their keyboard and accessibility focus disabled.',
    render: function (): React.Node {
      return (
        <View testID="accessible_pressable" accessible>
          <Pressable
            style={{
              height: 50,
            }}
            onPress={() => {}}>
            <Text>Pressable with accessible=true and focusable=true</Text>
          </Pressable>
          <Pressable
            style={{
              height: 50,
            }}
            onPress={() => {}}
            accessible={false}>
            <Text>Pressable with accessible=false</Text>
          </Pressable>
          <Pressable
            style={{
              height: 50,
            }}
            onPress={() => {}}
            focusable={false}>
            <Text>Pressable with focusable=false</Text>
          </Pressable>
          <Pressable
            style={{
              height: 30,
            }}
            onPress={() => {}}
            accessible={false}
            focusable={false}>
            <Text>Pressable with accessible=false and focusable=false</Text>
          </Pressable>
        </View>
      );
    },
  },
  {
    title: 'Pressable with ToolTip and NativeID',
    description: 'Pressables can have tooltips and nativeID.',
    render: function (): React.Node {
      return (
        <View>
          <Pressable
            testID="tooltip_pressable"
            style={{
              borderRadius: 2,
            }}
            onPress={() => {}}
            tooltip="Pressable"
            nativeID="Pressable-NativeID">
            <Text>Pressable with ToolTip "Pressable"</Text>
          </Pressable>
        </View>
      );
    },
  },
  {
    title: 'Pressable with no-hide-accessibility',
    description: 'Pressables can hide accessibility focus for their children.',
    render: function (): React.Node {
      return (
        <View>
          <Pressable
            testID="no_hide_pressable"
            onPress={() => {}}
            importantForAccessibility="no-hide-descendants"
            accessibilityHint="Button"
            accessibilityValue={{text: 'Child Pressable'}}>
            <Text>Parent Pressable</Text>
            <Pressable onPress={() => {}}>
              <Text>Child Pressable</Text>
            </Pressable>
          </Pressable>
        </View>
      );
    },
  },
  {
    title: 'Pressable with backfaceVisibility',
    description: 'Pressables can hide their backface.',
    render: function (): React.Node {
      return (
        <View testID="backface_pressable" accessible>
          <Text style={{paddingBottom: 10}}>
            View #1, front is visible, back is hidden.
          </Text>
          <View style={{justifyContent: 'center', alignItems: 'center'}}>
            <Pressable
              style={{
                height: 200,
                width: 200,
                justifyContent: 'center',
                alignItems: 'center',
                backgroundColor: 'blue',
                backfaceVisibility: 'hidden',
              }}>
              <Text>Front</Text>
            </Pressable>
            <Pressable
              style={{
                height: 200,
                width: 200,
                justifyContent: 'center',
                alignItems: 'center',
                backgroundColor: 'red',
                backfaceVisibility: 'hidden',
                transform: [{rotateY: '180deg'}],
                position: 'absolute',
                top: 0,
              }}>
              <Text>Back (You should not see this)</Text>
            </Pressable>
          </View>
          <Text style={{paddingVertical: 10}}>
            View #2, front is hidden, back is visible.
          </Text>
          <View style={{justifyContent: 'center', alignItems: 'center'}}>
            <Pressable
              style={{
                height: 200,
                width: 200,
                justifyContent: 'center',
                alignItems: 'center',
                backgroundColor: 'blue',
                backfaceVisibility: 'hidden',
              }}>
              <Text>Front (You should not see this)</Text>
            </Pressable>
            <Pressable
              style={{
                height: 200,
                width: 200,
                justifyContent: 'center',
                alignItems: 'center',
                backgroundColor: 'red',
                backfaceVisibility: 'hidden',
                position: 'absolute',
                top: 0,
              }}>
              <Text>Back</Text>
            </Pressable>
          </View>
        </View>
      );
    },
  },
  // [Windows
  {
    title: 'Set of Pressables',
    description:
      'Pressables can be defined as a set using accessibilityPosInSet and accessibilitySetSize',
    render: function (): React.Node {
      return (
        <View accessible testID="pressable_set">
          <Pressable
            onPress={() => {}}
            accessibilityPosInSet={1}
            accessibilitySetSize={3}
            accessibilityLiveRegion="none">
            <Text>Pressable 1 of 3</Text>
          </Pressable>
          <Pressable
            onPress={() => {}}
            accessibilityPosInSet={2}
            accessibilitySetSize={3}
            accessibilityLiveRegion="polite">
            <Text>Pressable 2 of 3</Text>
          </Pressable>
          <Pressable
            onPress={() => {}}
            accessibilityPosInSet={3}
            accessibilitySetSize={3}
            accessibilityLiveRegion="assertive">
            <Text>Pressable 3 of 3</Text>
          </Pressable>
        </View>
      );
    },
  },
  // Windows]
];

if (ReactNativeFeatureFlags.shouldPressibilityUseW3CPointerEventsForHover()) {
  examples.push({
    title: 'Change style based on Hover',
    render(): React.Node {
      return <PressableHoverStyle />;
    },
  });
}

module.exports = ({
  title: 'Pressable',
  documentationURL: 'https://reactnative.dev/docs/pressable',
  category: 'UI',
  description: 'Component for making views pressable.',
  displayName: 'Pressable',
  examples,
}: RNTesterModule);
