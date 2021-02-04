'use strict';
import * as React from 'react';
import { StyleSheet, Text, TextInput, View } from 'react-native';

// Disabling no-jsx-lambda so functional components are more convenient to use

const AutoFocusingTextInputTest: React.FC<{}> = () => {
  return (
    <View>
      <Text>This TextInput auto focuses</Text>
      <TextInput
        autoFocus
        maxLength={140}
        multiline
        style={styles.input}
      />
    </View>
  );
};

const PlaceholderTextInputTest: React.FC<{}> = () => {
  return (
    <View>
      <Text>This TextInput demonstrates placeholders</Text>
      <TextInput
        maxLength={140}
        multiline
        placeholder="I am a blue placeholder, type in me for something magical"
        placeholderTextColor="blue"
        style={styles.green}
      />
    </View>
  );
};

const ControllingTextInputTest: React.FC<{}> = () => {
  const [value, setValue] = React.useState('');

  return (
    <View>
      <Text>This TextInput inserts spaces between characters</Text>
      <TextInput
        multiline
        onChangeText={(text) => setValue(text.endsWith(' ') ? value : (text + ' '))}
        style={styles.blue}
        value={value}
      />
    </View>
  );
};

const BlurringAndFocusingTextInputTest: React.FC<{}> = () => {
  const [isFocused, setIsFocused] = React.useState(false);
  return (
    <View>
      <Text>This TextInput is currently focused: + {isFocused ? 'true' : 'false'}</Text>
      <TextInput
        multiline
        onBlur={() => setIsFocused(false)}
        onFocus={() => setIsFocused(true)}
        style={styles.input}
      />
    </View>
  );
};

const LayoutListeningTextInputTest: React.FC<{}> = () => {
  const [eventCounter, setEventCounter] = React.useState(0);

  return (
    <View>
      <Text>The number of onContentSizeChanges is listed here + {eventCounter}</Text>
      <TextInput
        multiline
        onContentSizeChange={() => setEventCounter(eventCounter + 1)}
        style={styles.input}
      />
    </View>
  );
};

const KeyPressListeningTextInputTest: React.FC<{}> = () => {
  const [value, setValue] = React.useState('');

  return (
    <View>
      <Text>This TextInput uses onKeyPress to maintain state: it is slow</Text>
      <TextInput
        multiline
        onKeyPress={(e) => setValue(value + e.nativeEvent.key)}
        style={styles.input}
        value={value}
      />
    </View>
  );
};

const StyleTextInputTest: React.FC<{}> = () => {
  return (
    <View>
      <Text>This TextInput is styled differently</Text>
      <TextInput
        multiline
        style={styles.superInput}
      />
    </View>
  );
};

export const title = 'TextInput';
export const displayName = 'TextInput';
export const description = 'TextInput Examples and Tests';
export const examples = [
    {
      title: 'Autofocus Example',
      description: 'autoFocus in action',
      render(): JSX.Element {
        return (<AutoFocusingTextInputTest />);
      },
    },
    {
      title: 'Placeholders Example',
      description: 'placeholder in action',
      render(): JSX.Element {
        return (<PlaceholderTextInputTest />);
      },
    },
    {
      title: 'Controlled Example',
      description: 'Controlling inputs in action',
      render(): JSX.Element {
        return (<ControllingTextInputTest />);
      },
    },
    {
      title: 'Focus and Blur Example',
      description: 'onFocus/onBlur in action',
      render(): JSX.Element {
        return (<BlurringAndFocusingTextInputTest />);
      },
    },
    {
      title: 'ContentSizeChange Example',
      description: 'onContentSizeChange in action',
      render(): JSX.Element {
        return (<LayoutListeningTextInputTest />);
      },
    },
    {
      title: 'Control via onKeyPress Example',
      description: 'onKeyPress in action',
      render(): JSX.Element {
        return (<KeyPressListeningTextInputTest />);
      },
    },
    {
      title: 'Super Styling Example',
      description: 'Styling in action',
      render(): JSX.Element {
        return (<StyleTextInputTest />);
      },
    },
  ];

const styles = StyleSheet.create({
  input: {
    height: 20,
  },
  blue: {
    color: 'blue',
    height: 20,
  },
  green: {
    color: 'green',
    height: 20,
  },
  superInput: {
    color: 'purple',
    fontSize: 24,
    fontStyle: 'italic',
    fontWeight: 'bold',
    fontFamily: 'times-new-roman',
    height: 100,
  },
});
