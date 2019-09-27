/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import { Picker } from 'react-native-windows';
import {
  AppRegistry,
  Image,
  View,
  Text,
  Switch,
  StyleSheet,
  Animated,
  Easing,
} from 'react-native';

const largeImageUri = 'https://cdn.freebiesupply.com/logos/large/2x/react-logo-png-transparent.png';
const smallImageUri = 'http://facebook.github.io/react-native/img/header_logo.png';

export default class Bootstrap extends Component {
  state = {
        animatedTranslation: new Animated.Value(0),
  };

  shiftSteps = (numStepsLeft) => {
    if (numStepsLeft > 0) {
        Animated.timing(this.state.animatedTranslation, {
            toValue: (6 - numStepsLeft) * 100,
            duration: 1000,
            easing: Easing.linear,
            useNativeDriver: true, 
        }).start(() => {
            this.shiftSteps(numStepsLeft - 1);
        });
    }
  };

  componentDidMount()
  {
    this.shiftSteps(5);
  }

  render() {
    return (
      <View style={styles.contents}>
          <Animated.View
            style={[
              styles.shiftingContainer,
              {
                transform: [
                  {
                    translateX: this.state.animatedTranslation,
                  },
                ],
              },
            ]}>
            <Text style={styles.shiftingText}>This should slide smoothly 500px to the right!</Text>
          </Animated.View>
      </View>
    );
  }
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


// /**
//  * Sample React Native App
//  * https://github.com/facebook/react-native
//  * @flow
//  */

// import React, { Component } from 'react';
// import { Picker } from 'react-native-windows';
// import {
//   AppRegistry,
//   Image,
//   View,
//   Text,
//   Switch,
//   StyleSheet,
//   Animated,
// } from 'react-native';

// const largeImageUri = 'https://cdn.freebiesupply.com/logos/large/2x/react-logo-png-transparent.png';
// const smallImageUri = 'http://facebook.github.io/react-native/img/header_logo.png';

// export default class Bootstrap extends Component {
//   state = {
//     animatedTranslation: Animated.Value(0),
//     shiftingContainer: {
//         ...styles.shiftingContainer,
//         transform: [{ translateX: animatedTranslation }],
//     },
//   };

//   // shiftSteps = (numStepsLeft) => {
//   //   if (numStepsLeft > 0) {
//   //       Animated.timing(animatedTranslation, {
//   //           toValue: (6 - numStepsLeft) * 100,
//   //           duration: 1000,
//   //           easing: Easing.linear,
//   //           useNativeDriver: false, 
//   //       }).start(() => {
//   //           shiftSteps(numStepsLeft - 1);
//   //       });
//   //   }
//   // };

//   // componentDidMount() {
//   //   shiftSteps(5);
//   // }

//   render() {
//     return (
//       <View style={styles.contents}>
//           <Animated.View style={shiftingContainer}>
//               <Text style={styles.shiftingText}>This should slide smoothly 500px to the right!</Text>
//           </Animated.View>
//       </View>
//     );
//   }
// }

// const styles = StyleSheet.create({
//   contents: {
//       height: 1080,
//       width: 1920,
//       alignItems: "center",
//       justifyContent: "center",
//   },
//   shiftingText: {
//       fontSize: 26,
//   },
//   shiftingContainer: {
//       alignItems: "center",
//       justifyContent: "center",
//   },
// });

// AppRegistry.registerComponent('Bootstrap', () => Bootstrap);

