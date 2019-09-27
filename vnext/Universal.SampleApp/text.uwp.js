/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React, {Component} from 'react';
import {AppRegistry, StyleSheet, Text, View, useState, useEffect, Animated} from 'react-native';

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

export const TranslatingModule = () => {
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
      <View style={styles.contents}>
          <Animated.View style={shiftingContainer}>
              <Text style={styles.shiftingText}>This should slide smoothly 500px to the right!</Text>
          </Animated.View>
      </View>
  );
};

AppRegistry.registerComponent('Bootstrap', () => TranslatingModule);
