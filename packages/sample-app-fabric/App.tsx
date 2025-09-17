/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React from 'react';
import {View, SafeAreaView, StatusBar, useColorScheme, ScrollView} from 'react-native';
import {NewAppScreen} from '@react-native/new-app-screen';
import XamlHost from 'react-native-windows/Libraries/Components/Xaml/XamlHost';

function App(): React.JSX.Element {
  const isDarkMode = useColorScheme() === 'dark';

  return (
    <SafeAreaView style={{flex: 1}}>
      <StatusBar
        barStyle={isDarkMode ? 'light-content' : 'dark-content'}
        backgroundColor="transparent"
        translucent
          />
          <XamlHost label="sharath3" style={{ height:200, borderWidth: 2, borderColor: 'yellow' }} />
          <NewAppScreen />
    </SafeAreaView>
  );
}

export default App;
