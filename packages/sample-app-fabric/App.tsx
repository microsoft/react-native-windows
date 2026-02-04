/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, {useState} from 'react';
import {
  SafeAreaView,
  StatusBar,
  useColorScheme,
  Text,
  View,
} from 'react-native';
import {NewAppScreen} from '@react-native/new-app-screen';
import {PickerXaml} from 'sample-custom-component';

const pickerItems = [
  {label: 'JavaScript', value: 'js'},
  {label: 'TypeScript', value: 'ts'},
  {label: 'Python', value: 'py'},
  {label: 'C++', value: 'cpp'},
  {label: 'Rust', value: 'rs'},
];

function App(): React.JSX.Element {
  const isDarkMode = useColorScheme() === 'dark';
  const [selectedIndex, setSelectedIndex] = useState(3); // Default to C++

  const backgroundColor = isDarkMode ? '#1a1a1a' : '#f5f5f5';
  const textColor = isDarkMode ? '#ffffff' : '#000000';

  return (
    <SafeAreaView style={{flex: 1, backgroundColor}}>
      <StatusBar
        barStyle={isDarkMode ? 'light-content' : 'dark-content'}
        backgroundColor="transparent"
        translucent
      />
      <View style={{padding: 20, backgroundColor, borderRadius: 8, margin: 10}}>
        <Text style={{fontSize: 16, marginBottom: 10, color: textColor}}>
          Selected: {pickerItems[selectedIndex]?.label}
        </Text>
        <PickerXaml
          style={{width: 200, height: 40}}
          items={pickerItems}
          selectedIndex={selectedIndex}
          onPickerSelect={event => {
            setSelectedIndex(event.nativeEvent.itemIndex);
          }}
        />
      </View>
      <NewAppScreen />
    </SafeAreaView>
  );
}

export default App;
