/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  TextInput,
  View,
  AccessibilityInfo,
  findNodeHandle,
  TouchableHighlight,
  TouchableOpacity,
  TouchableWithoutFeedback,
  Switch,
} from 'react-native';
import {ViewWindows} from 'react-native-windows';

export default class Bootstrap extends React.Component<
  {},
  {displayText: string; counterValue: number; sliderValue: number}
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      displayText: 'Starting text. (THIRD ITEM)',
      counterValue: 0,
      sliderValue: 0,
    };
  }

  myElement = React.createRef<TextInput>();

  render() {
    return (
      <View
        accessibilityHint="topmost view"
        //importantForAccessibility="no-hide-descendants"
        style={styles.container}>
        <View
          importantForAccessibility="no-hide-descendants"
          accessibilityLabel="FIRST ITEM">
          <Text style={styles.text}>Welcome to React Native! (FIRST ITEM)</Text>
        </View>
        <TouchableHighlight
          importantForAccessibility="no-hide-descendants"
          accessibilityRole="adjustable"
          accessibilityValue={{
            min: 0,
            max: 100,
            now: this.state.sliderValue,
          }}
          onPress={() => {
            this.setState({sliderValue: this.state.sliderValue + 1});
          }}>
          <Text style={styles.text}>
            Testing acessibilityValue:min/max/now, click to increase:{' '}
            {this.state.sliderValue}
          </Text>
        </TouchableHighlight>
        <TextInput
          importantForAccessibility="no-hide-descendants"
          //accessible={false}
          ref={this.myElement}
        />
        <Switch importantForAccessibility="no-hide-descendants" />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: 'lightgray',
  },
  item: {
    margin: 10,
    backgroundColor: 'lightpink',
    borderColor: 'indianred',
    borderWidth: 2,
    justifyContent: 'center',
  },
  text: {
    fontSize: 20,
    textAlign: 'center',
    color: 'black',
    margin: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
