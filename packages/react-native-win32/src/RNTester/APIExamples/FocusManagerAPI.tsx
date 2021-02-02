import * as React from 'react';
import { FocusManager } from '../../Libraries/Utilities/FocusManager.win32';
import { View, StyleSheet, TouchableHighlight, Text } from 'react-native';

const styles = StyleSheet.create({
    border: {
      borderStyle: 'dotted',
      borderColor: 'black',
    },
    keyComponentRoot: {
      borderWidth: 2,
      flexDirection: 'row',
      marginVertical: 5,
      backgroundColor: 'whitesmoke',
      justifyContent: 'space-around',
    },
    keyEnterVisualizer: {
      margin: 5,
      alignItems: 'center',
      minWidth: 100,
      minHeight: 30,
    },
    blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 },
  });

export const PoliteFocusTest: React.FunctionComponent<{}> = () => {
    const buttonRef = React.useRef();
    const buttonRef2 = React.useRef<View>();
    
    const politeFocusOnPress = () => {
      FocusManager.focus(buttonRef.current, false);
    }
  
    const aggresiveFocusOnPress = () => {
      FocusManager.focus(buttonRef.current, true);
    }
  
    return (
        <View>
          <Text>Polite Focus Test</Text>
          <View style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 }}>
            <TouchableHighlight onPress={politeFocusOnPress}>
              <View ref={buttonRef} style={styles.blackbox} />
            </TouchableHighlight>
          </View>
          <Text>Aggressive Focus Test</Text>
          <View style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 }}>
            <TouchableHighlight onPress={aggresiveFocusOnPress}>
              <View ref={buttonRef2} style={styles.blackbox} />
            </TouchableHighlight>
          </View>        
        </View>
    );
  };

  export const title = 'FocusManager APIs';
  export const displayName =  'FocusManager APIs';
  export const description = 'Tests FocusManager';
  export const examples = [
    {
      title: 'FocusManager Example',
      description: 'All constants',
      render: () => <PoliteFocusTest />
    }
  ];