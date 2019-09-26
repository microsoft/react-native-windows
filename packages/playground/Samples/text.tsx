/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

// import * as React from 'react';
// import {AppRegistry, StyleSheet, Text, View} from 'react-native';

// export default class Bootstrap extends React.Component {
//   render() {
//     return (
//       <View style={styles.container}>
//         <Text style={styles.welcome}>Welcome to React Native!</Text>
//       </View>
//     );
//   }
// }

// const styles = StyleSheet.create({
//   container: {
//     flex: 1,
//     justifyContent: 'center',
//     alignItems: 'center',
//     backgroundColor: '#C5CCFF',
//   },
//   welcome: {
//     fontSize: 20,
//     textAlign: 'center',
//     margin: 10,
//   },
// });

// AppRegistry.registerComponent('Bootstrap', () => Bootstrap);

import * as React from "react";
import { useState, useEffect } from "react";
import { StyleSheet, View, Text, Animated, Easing, AppRegistry, Button} from "react-native";

// const styles = StyleSheet.create({
//     contents: {
//         height: 1080,
//         width: 1920,
//         alignItems: "center",
//         justifyContent: "center",
//     },
//     fadingText: {
//         fontSize: 26,
//     },
//     fadingContainer: {
//         alignItems: "center",
//         justifyContent: "center",
//     },
// });

// export const FaderModule = () => {
//     const [animatedOpacity] = useState(new Animated.Value(1));
//     const [fadingContainer] = useState({
//         ...styles.fadingContainer,
//         opacity: animatedOpacity,
//     });

//     const fade = () => {
//         animatedOpacity.stopAnimation();
//         Animated.timing(animatedOpacity, {
//             toValue: 0,
//             duration: 5000,
//             easing: Easing.linear,
//             useNativeDriver: true,
//         }).start();
//     };

//     useEffect(fade, []);

//     return (
//         <View style={styles.contents}>
//             <Animated.View style={fadingContainer}>
//                 <Text style={styles.fadingText}>This should fade!</Text>
//             </Animated.View>
//             <Button onPress={() => {
//                   fade();
//                 }} title="Press to Fade">
//                   <Text>"Press to Fade"</Text>
//             </Button>
//         </View>
//     );
// };
 
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

  const shiftSteps = (numStepsLeft: number) => {
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