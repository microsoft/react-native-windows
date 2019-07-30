/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
import {
  AppRegistry,
  StyleSheet,
  View,
  Text,
  GestureResponderEvent,
  TouchableHighlight,
} from 'react-native';

export default class Bootstrap extends React.Component<
  {},
  {
    clicked: number;
    mouseEntered0: boolean;
    mouseEntered1: boolean;
    mouseEntered2: boolean;
  }
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      clicked: 0,
      mouseEntered0: false,
      mouseEntered1: false,
      mouseEntered2: false,
    };
  }

  render() {
    return (
      <View
        style={styles.container}
        {...{
          // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
          onMouseEnter: this.mouseEnter0,
          onMouseLeave: this.mouseLeave0,
        }}>
        <View
          style={{
            backgroundColor: this.state.mouseEntered1 ? 'yellow' : 'green',
          }}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            onMouseEnter: this.mouseEnter1,
            onMouseLeave: this.mouseLeave1,
          }}>
          <TouchableHighlight
            onPress={this.press}
            onPressIn={this.pressIn}
            onPressOut={this.pressOut}
            style={{
              backgroundColor: this.state.mouseEntered2
                ? 'blue'
                : 'transparent',
            }}
            {...{
              // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
              onMouseEnter: this.mouseEnter2,
              onMouseLeave: this.mouseLeave2,
            }}>
            <Text>World</Text>
          </TouchableHighlight>
          <Text>Hello</Text>
        </View>
        <View
          style={{
            backgroundColor: this.state.mouseEntered0 ? 'green' : 'transparent',
          }}>
          <Text>Mousey</Text>
        </View>
      </View>
    );
  }

  click = () => {
    this.setState({clicked: this.state.clicked + 1});
  };
  mouseEnter0 = () => {
    this.setState({mouseEntered0: true});
  };

  mouseLeave0 = () => {
    this.setState({mouseEntered0: false});
  };

  mouseEnter1 = () => {
    this.setState({mouseEntered1: true});
  };

  mouseLeave1 = () => {
    this.setState({mouseEntered1: false});
  };
  mouseEnter2 = () => {
    this.setState({mouseEntered2: true});
  };

  mouseLeave2 = () => {
    this.setState({mouseEntered2: false});
  };
  press = (event: GestureResponderEvent) => {
    console.log('press');
    console.log(event);
  };
  pressIn = (event: GestureResponderEvent) => {
    console.log('pressin');
    console.log(event);
  };
  pressOut = (event: GestureResponderEvent) => {
    console.log('pressout');
    console.log(event);
  };
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#C5CCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
