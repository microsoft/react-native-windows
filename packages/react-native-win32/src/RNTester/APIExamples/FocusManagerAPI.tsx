/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import { FocusManager } from '../../Libraries/Utilities/FocusManager.win32';
import { View, StyleSheet, TouchableHighlight, Text } from 'react-native';

const styles = StyleSheet.create({
    blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 },
  });

export const PoliteFocusTest: React.FunctionComponent<{}> = () => {
    const [isPoliteFocus, changePoliteFocus] = React.useState('Not focused');
    const [isAggressiveFocus, changeAggressiveFocus] = React.useState('Not focused');

    const buttonRef = React.useRef();
    const buttonRef2 = React.useRef();
    
    const politeFocusOnPress = () => {
      FocusManager.focus(buttonRef.current, false);
      changePoliteFocus('Polite Focus Set');
    }
  
    const aggresiveFocusOnPress = () => {
      FocusManager.focus(buttonRef.current, true);
      changeAggressiveFocus('Aggressive Focus Set');
    }
  
    return (
        <View style={{ flexDirection: 'row', alignItems: 'center', marginVertical: 5 }}>
          <View style={{ flexDirection: 'column', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 }}>
            <Text>Polite Focus Test</Text>
            <TouchableHighlight onPress={politeFocusOnPress}>
              <View ref={buttonRef} style={styles.blackbox} />
            </TouchableHighlight>
            <Text>{isPoliteFocus}</Text>
          </View>
          <View style={{ flexDirection: 'column', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5, marginLeft: 10 }}>
            <Text>Aggressive Focus Test</Text>
            <TouchableHighlight onPress={aggresiveFocusOnPress}>
              <View ref={buttonRef2} style={styles.blackbox} />  
            </TouchableHighlight>
            <Text>{isAggressiveFocus}</Text>
          </View>        
        </View>
    );
  };

  export const title = 'FocusManager API';
  export const displayName =  'FocusManager API Example';
  export const description = 'Change between polite and aggressive focus';
  export const examples = [
    {
      title: 'FocusManager Example',
      description: 'Use FocusManager API to change between polite and aggressive focus',
      render: () => <PoliteFocusTest />
    }
  ];