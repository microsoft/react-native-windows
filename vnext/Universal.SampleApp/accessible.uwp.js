/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  TextInput,
  View,
  useState,
  useEffect,
  Animated,
} from 'react-native';


export default class Bootstrap extends Component {
  constructor(props) {
    super(props);
    this.state = { animatedTranslation: Animated.Value(0), shiftingContainer: {...style.shiftingContainer, transform: [{ translateX: animatedTranslation}], }};
  }

export const Bootstrap = () => {
  const [animatedTranslation] = useState(new Animated.Value(0));
  const [shiftingContainer] = useState({
      ...styles.shiftingContainer,
      transform: [{ translateX: animatedTranslation }],
  });

  const shiftSteps = (numStepsLeft) => {
    if (numStepsLeft > 0) {
        Animated.timing(animatedTranslation, {
            toValue: (6 - numStepsLeft) * 100,
            duration: 1000,
            easing: Easing.linear,
            useNativeDriver: false, 
        }).start(() => {
            shiftSteps(numStepsLeft - 1);
        });
    }
  };

  useEffect(() => {
    shiftSteps(5);
  }, []);

    return (
      <View style={styles.container}>
        <View style={styles.item} acceptsKeyboardFocus={true} accessibilityLabel="FIRST ITEM">
          <Text style={styles.text}>Welcome to React Native! (FIRST ITEM)</Text>
        </View>
        <View style={styles.item} acceptsKeyboardFocus={true} accessibilityLabel="SECOND ITEM" enableFocusRing={false}>
          <Text style={styles.text}>No focus visual (SECOND ITEM)</Text>
        </View>
        <View style={styles.item} acceptsKeyboardFocus={true} accessibilityLabel="THIRD ITEM"
          onFocus={() => this.setState({ displayText: 'FOCUSED' })}
          onBlur={() => this.setState({ displayText: 'BLURRED' })}>
          <Text style={styles.text}>{this.state.displayText}</Text>
        </View>
        <TextInput/>
      </View>
    );
  }

const styles = StyleSheet.create({
  contents: {
      height: 1080,
      width: 1920,
      alignItems: "center",
      justifyContent: "center",
  },
  shiftingText: {
      fontSize: 26,
  },
  shiftingContainer: {
      alignItems: "center",
      justifyContent: "center",
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
