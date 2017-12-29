/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  View,
  Button,
  ScrollView,
  Alert,
  Animated,
  TextInput,
} from 'react-native';

import {
  FocusableWindows
} from 'react-native-windows';

FVWAnim = Animated.createAnimatedComponent(FocusableWindows);

class CustomControl extends Component {

  render () {

    return (
      <FocusableWindows
          onKeyUp={this._onKeyUp}
          isTabStop={true}
          tabIndex={0}
      >
        <Animated.View {...this.props}
        >
          {this.props.children}
        </Animated.View>
      </FocusableWindows>
    );
  }

  _onKeyUp = (e) => {
    if (e.nativeEvent.key === FocusableWindows.keys.Space) {
      Alert.alert("AHA", "Custom control pressed");
    }
  };
}

CustomControlAnim = Animated.createAnimatedComponent(CustomControl);

var bingoAnim = new Animated.Value(40);

class Playground extends Component {

  state = {
    widthAnim: new Animated.Value(40),
    jsAnimated: new Animated.Value(0),
    nativeAnimated: new Animated.Value(0),
    nativeAnimated2: new Animated.Value(0),
  };

  componentDidMount() {
    Animated.timing(                  // Animate over time
      this.state.widthAnim,           // The animated value to drive
      {
        toValue: 300,                 // Animate to position
        duration: 2000,               // Make it take a while
      }
    ).start();                        // Starts the animation

    Animated.timing(                  // Animate over time
      this.state.jsAnimated,           // The animated value to drive
      {
        toValue: 540,                 // Animate to position
        duration: 2000,               // Make it take a while
      }
    ).start();                        // Starts the animation

    Animated.timing(                  // Animate over time
      this.state.nativeAnimated,           // The animated value to drive
      {
        toValue: 540,                 // Animate to position
        duration: 2000,               // Make it take a while
        useNativeDriver: true,
      }
    ).start();                        // Starts the animation
  }

  render() {

    let { widthAnim, jsAnimated, nativeAnimated, nativeAnimated2 } = this.state;

    let styleJsAnimated =  {
      transform: [{
        translateY: jsAnimated
        }],
      backgroundColor: '#DDDDDD',
    };

    let styleNativeAnimated = {
      transform: [{
        translateY: nativeAnimated
        }],
      backgroundColor: '#DDDDDD',
    };

    let styleNotAnimated = {
      transform: [{
        translateY: 540
        }],
      backgroundColor: '#DDDDDD',
    };

    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>
          Welcome to React Native!
        </Text>
        <Text style={styles.instructions}>
          To get started, edit index.windows.js
        </Text>
        <Text style={styles.instructions}>
          Shake or press Shift+F10 for dev menu
        </Text>
        <Button
          title={"BTN 1"}
          onPress={this.onButtonPress}
        >
        </Button>
        <Button
          title={"BTN 2 (DIS)"}
          onPress={this.onButtonPress}
          disabled={true}
        >
        </Button>
        <Button
          title={"BTN 3"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN 4"}
          onPress={this.onButtonPress}
          disableSystemFocusVisuals={true}
          >
        </Button>
        <Button
          title={"BTN 5 (NOT FOCUSABLE)"}
          onPress={this.onButtonPress}
          tabIndex={-1}
        >
        </Button>

        <Animated.View
          style = { {
              minWidth: widthAnim,
              backgroundColor: '#DDDDDD',
          } }
          >
          <FocusableWindows
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
          >
            <View>
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (width, outside fvw, js)
              </Text>
            </View>
          </FocusableWindows>
        </Animated.View>

        <View>
          <FocusableWindows
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
          >
            <Animated.View
              style = { {
              minWidth: widthAnim,
              backgroundColor: '#DDDDDD',
              } }
            >
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (width, inside fvw, js)
              </Text>
            </Animated.View>
          </FocusableWindows>
        </View>

        <View>
            <Animated.View
              style = { {
              minWidth: widthAnim,
              backgroundColor: '#DDDDDD',
              } }
            >
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (width, no fvw, js)
              </Text>
            </Animated.View>
        </View>
        <View>
            <Animated.View
              style = { {
              minWidth: 40,
              backgroundColor: '#DDDDDD',
              } }
            >
              <Text onPress={this.onButtonPress} style={styles.instructions}>
               No  Animation (width, no fvw, js)
              </Text>
            </Animated.View>
        </View>        
        <Animated.View
          style = { styleJsAnimated }
          >
          <FocusableWindows
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
          >
            <View>
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (translateY, outside fvw, js)
              </Text>
            </View>
          </FocusableWindows>
        </Animated.View>

        <Animated.View
          style = { styleNativeAnimated }
          >
          <FocusableWindows
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
          >
            <View>
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (translateY, outside fvw, native)
              </Text>
            </View>
          </FocusableWindows>
        </Animated.View>

        <View>
          <FocusableWindows
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
            >
            <Animated.View
              style = { styleNotAnimated }
              >
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                No Animation (translateY, inside fvw)
              </Text>
            </Animated.View>
          </FocusableWindows>
        </View>

        <View>
          <FVWAnim
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
            >
            <Animated.View
              style = { styleJsAnimated }
            >
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (translateY, fvw itself is animated, js)
              </Text>
            </Animated.View>
          </FVWAnim>
        </View>

        <View>
          <FVWAnim
            onKeyUp={this.onKeyUp}
            isTabStop={true}
            tabIndex={0}
            >
            <Animated.View
              style = { styleNativeAnimated }
            >
              <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (translateY, fvw itself is animated, native)
              </Text>
            </Animated.View>
          </FVWAnim>
        </View>

        <CustomControl style={styleNotAnimated}>
            <Text onPress={this.onButtonPress} style={styles.instructions}>
                No Animation (translateY, custom fwv-based control)
            </Text>
        </CustomControl>

       <CustomControlAnim style={styleJsAnimated}>
            <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (translateY, custom fwv-based control, js)
            </Text>
        </CustomControlAnim>

        <CustomControlAnim style={styleNativeAnimated}>
            <Text onPress={this.onButtonPress} style={styles.instructions}>
                Animation (translateY, custom fwv-based control, native)
            </Text>
        </CustomControlAnim>

        <Text style={styles.instructions}>
          Regular text
        </Text>

        <Text style={styles.instructions} onPress={this.onButtonPress}>
          Pressable text
        </Text>

        <FocusableWindows
          onKeyUp={this.onKeyUp}
          isTabStop={true}
          tabIndex={0}
          >
        <Text style={styles.instructions} onPress={this.onButtonPress}>
          Pressable and focusable text
        </Text>
        </FocusableWindows>

        <TextInput style={styles.input} secureTextEntry={true} onKeyUp=
            {
              (e) =>
              this.onButtonPress(e)
              }/>

        <ScrollView showsVerticalScrollIndicator={true} style={styles.scrollViewer} onKeyDown={this.onKeyDown}>
        <Button
          title={"BTN s1"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s2"}
          onPress={this.onButtonPress}
          disabled={true}
          >
        </Button>
        <Button
          title={"BTN s3"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s4"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s5"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s6"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s7"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s8"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s9"}
          onPress={this.onButtonPress}
          >
        </Button>
        <Button
          title={"BTN s10"}
          onPress={this.onButtonPress}
          >
        </Button>
        </ScrollView>

      </View>
    );
  }

  onButtonPress = function (e) {
    Alert.alert("AHA", "Button pressed");
  };

  onKeyDown = function (e) {
    //Alert.alert("AHA", "Key Down Phase:" + e.eventPhase + " ct:" + e.currentTarget + " t:" + e.target + " n:" + e.nativeEvent);
  };

  onKeyUp = function (e) {
    if (e.nativeEvent.key === 32) {
      Alert.alert("AHA", "Text kbd pressed");
    }
  };
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'flex-start',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
  scrollViewer: {
    maxHeight: 100,
    margin: 5,
  },
  testAnim: {
    minWidth: 40,
    backgroundColor: '#DDDDDD',
  },
  input: {
    minWidth: 60,
    maxWidth: 100,
    backgroundColor: '#DDDDDD',
  }
});

AppRegistry.registerComponent('Playground.Net46', () => Playground);
