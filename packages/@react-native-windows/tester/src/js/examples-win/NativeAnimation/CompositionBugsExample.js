/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

const React = require('react');

const {
  View,
  Text,
  Animated,
  StyleSheet,
  TouchableWithoutFeedback,
} = require('react-native');

class Tester extends React.Component<$FlowFixMeProps, $FlowFixMeState> {
  state = {
    native: new Animated.Value(0),
    tick: new Animated.Value(0),
    js: new Animated.Value(0),
  };

  current = 0;

  onPress = () => {
    const animConfig =
      this.current && this.props.reverseConfig
        ? this.props.reverseConfig
        : this.props.config;
    this.current = this.current ? 0 : 1;
    const config: Object = {
      ...animConfig,
      toValue: this.current,
    };

    Animated[this.props.type](this.state.native, {
      ...config,
      useNativeDriver: true,
    }).start();
    Animated[this.props.type](this.state.tick, {
      ...config,
      useNativeDriver: true,
      platformConfig: {
        useComposition: false,
      },
    }).start();
    Animated[this.props.type](this.state.js, {
      ...config,
      useNativeDriver: false,
    }).start();

    if (this.props.onPress) {
      this.props.onPress(this.state.native);
      this.props.onPress(this.state.tick);
      this.props.onPress(this.state.js);
    }
  };

  render() {
    return (
      <TouchableWithoutFeedback onPress={this.onPress}>
        <View>
          <View>
            <Text>Composition:</Text>
          </View>
          <View style={styles.row}>
            {this.props.children(this.state.native)}
          </View>
          <View>
            <Text>UI Tick{':'}</Text>
          </View>
          <View style={styles.row}>{this.props.children(this.state.tick)}</View>
          <View>
            <Text>JavaScript{':'}</Text>
          </View>
          <View style={styles.row}>{this.props.children(this.state.js)}</View>
        </View>
      </TouchableWithoutFeedback>
    );
  }
}

class ValueListenerTester extends Tester {
  state = {
    native: new Animated.Value(0),
    tick: new Animated.Value(0),
    js: new Animated.Value(0),
    nativeValue: 0,
    tickValue: 0,
    jsValue: 0,
  };

  componentDidMount() {
    this.state.native.addListener(e => this.setState({nativeValue: e.value}));
    this.state.tick.addListener(e => this.setState({tickValue: e.value}));
    this.state.js.addListener(e => this.setState({jsValue: e.value}));
  }

  componentWillUnmount() {
    this.state.native.removeAllListeners();
    this.state.tick.removeAllListeners();
    this.state.js.removeAllListeners();
  }

  render() {
    return (
      <TouchableWithoutFeedback onPress={this.onPress}>
        <View>
          <View>
            <Text>Composition:</Text>
          </View>
          <View style={styles.row}>
            {this.props.children(this.state.native)}
          </View>
          <Text>
            Value: {this.state.nativeValue}
            {'\n'}
          </Text>
          <View>
            <Text>UI Tick{':'}</Text>
          </View>
          <View style={styles.row}>{this.props.children(this.state.tick)}</View>
          <Text>
            Value: {this.state.tickValue}
            {'\n'}
          </Text>
          <View>
            <Text>JavaScript{':'}</Text>
          </View>
          <View style={styles.row}>{this.props.children(this.state.js)}</View>
          <Text>
            Value: {this.state.jsValue}
            {'\n'}
          </Text>
        </View>
      </TouchableWithoutFeedback>
    );
  }
}

class RevertToStaticPropsExample extends React.Component<
  $FlowFixMeProps,
  $FlowFixMeState,
> {
  state = {
    native: new Animated.Value(0),
    tick: new Animated.Value(0),
    js: new Animated.Value(0),
    resetProp: false,
  };

  current = 0;

  onPress = () => {
    if (this.current) {
      this.state.native.setValue(0);
      this.state.tick.setValue(0);
      this.state.js.setValue(0);
      this.setState({resetProp: true});
    } else {
      this.setState({resetProp: false});
      const config: Object = {
        ...this.props.config,
        toValue: 50,
      };

      Animated[this.props.type](this.state.native, {
        ...config,
        useNativeDriver: true,
      }).start();
      Animated[this.props.type](this.state.tick, {
        ...config,
        useNativeDriver: true,
        platformConfig: {
          useComposition: false,
        },
      }).start();
      Animated[this.props.type](this.state.js, {
        ...config,
        useNativeDriver: false,
      }).start();

      if (this.props.onPress) {
        this.props.onPress(this.state.native);
        this.props.onPress(this.state.tick);
        this.props.onPress(this.state.js);
      }
    }
    this.current = this.current ? 0 : 1;
  };

  render() {
    return (
      <TouchableWithoutFeedback onPress={this.onPress}>
        <View>
          <View>
            <Text>Composition:</Text>
          </View>
          <View style={styles.row}>
            {this.props.children(
              this.state.resetProp ? undefined : this.state.native,
            )}
          </View>
          <View>
            <Text>UI Tick{':'}</Text>
          </View>
          <View style={styles.row}>
            {this.props.children(
              this.state.resetProp ? undefined : this.state.tick,
            )}
          </View>
          <View>
            <Text>JavaScript{':'}</Text>
          </View>
          <View style={styles.row}>
            {this.props.children(
              this.state.resetProp ? undefined : this.state.js,
            )}
          </View>
        </View>
      </TouchableWithoutFeedback>
    );
  }
}

class StopAnimationCallbackTester extends Tester {
  onPress = () => {
    const animConfig =
      this.current && this.props.reverseConfig
        ? this.props.reverseConfig
        : this.props.config;
    this.current = this.current ? 0 : 1;
    const config: Object = {
      ...animConfig,
      toValue: this.current,
    };

    Animated[this.props.type](this.state.native, {
      ...config,
      useNativeDriver: true,
    }).start();
    Animated[this.props.type](this.state.tick, {
      ...config,
      useNativeDriver: true,
      platformConfig: {
        useComposition: false,
      },
    }).start();
    Animated[this.props.type](this.state.js, {
      ...config,
      useNativeDriver: false,
    }).start();

    setTimeout(() => {
      this.state.native.stopAnimation(nativeValue =>
        this.setState({nativeValue}),
      );
      this.state.tick.stopAnimation(tickValue => this.setState({tickValue}));
      this.state.js.stopAnimation(jsValue => this.setState({jsValue}));
    }, config.duration / 2);
  };

  render() {
    return (
      <TouchableWithoutFeedback onPress={this.onPress}>
        <View>
          <View>
            <Text>Composition:</Text>
          </View>
          <View style={styles.row}>
            {this.props.children(this.state.native)}
          </View>
          <Text>
            Final Value: {this.state.nativeValue}
            {'\n'}
          </Text>
          <View>
            <Text>UI Tick{':'}</Text>
          </View>
          <View style={styles.row}>{this.props.children(this.state.tick)}</View>
          <Text>
            Final Value: {this.state.tickValue}
            {'\n'}
          </Text>
          <View>
            <Text>JavaScript{':'}</Text>
          </View>
          <View style={styles.row}>{this.props.children(this.state.js)}</View>
          <Text>
            Final Value: {this.state.jsValue}
            {'\n'}
          </Text>
        </View>
      </TouchableWithoutFeedback>
    );
  }
}

const styles = StyleSheet.create({
  row: {
    padding: 10,
    zIndex: 1,
  },
  block: {
    width: 50,
    height: 50,
    backgroundColor: 'blue',
  },
  line: {
    position: 'absolute',
    left: 35,
    top: 0,
    bottom: 0,
    width: 1,
    backgroundColor: 'red',
  },
});

exports.framework = 'React';
exports.title = 'Composition Bugs Example';
exports.category = 'UI';
exports.description = 'See bugs in UI.Composition driven native animations';

exports.examples = [
  {
    title: 'Animated value listener',
    render: function (): React.Node {
      return (
        <ValueListenerTester type="timing" config={{duration: 1000}}>
          {anim => (
            <Animated.View
              style={[
                styles.block,
                {
                  opacity: anim.interpolate({
                    inputRange: [0, 1],
                    outputRange: [1, 0],
                  }),
                },
              ]}
            />
          )}
        </ValueListenerTester>
      );
    },
  },
  {
    title: "Arbitrary props (e.g., 'borderRadius')",
    render: function (): React.Node {
      return (
        <Tester type="timing" config={{duration: 1000}}>
          {anim => (
            <Animated.View
              style={[
                styles.block,
                {
                  borderRadius: anim.interpolate({
                    inputRange: [0, 1],
                    outputRange: [0, 25],
                  }),
                },
              ]}
            />
          )}
        </Tester>
      );
    },
  },
  {
    title: 'diffClamp',
    render: function (): React.Node {
      return (
        <Tester type="timing" config={{duration: 1000}}>
          {anim => (
            <Animated.View
              style={[
                styles.block,
                {
                  opacity: Animated.diffClamp(
                    anim.interpolate({
                      inputRange: [0, 1],
                      outputRange: [1, 0],
                    }),
                    0,
                    0.5,
                  ),
                },
              ]}
            />
          )}
        </Tester>
      );
    },
  },
  {
    title: 'setValue in active animation',
    render: function (): React.Node {
      return (
        <Tester
          type="timing"
          config={{duration: 1000}}
          onPress={anim => setTimeout(() => anim.setValue(0.5), 500)}>
          {anim => {
            return (
              <Animated.View
                style={[
                  styles.block,
                  {
                    opacity: anim.interpolate({
                      inputRange: [0, 1],
                      outputRange: [1, 0],
                    }),
                  },
                ]}
              />
            );
          }}
        </Tester>
      );
    },
  },
  {
    title: 'stopAnimation callback',
    render: function (): React.Node {
      return (
        <StopAnimationCallbackTester type="timing" config={{duration: 1000}}>
          {anim => {
            return (
              <Animated.View
                style={[
                  styles.block,
                  {
                    opacity: anim.interpolate({
                      inputRange: [0, 1],
                      outputRange: [1, 0],
                    }),
                  },
                ]}
              />
            );
          }}
        </StopAnimationCallbackTester>
      );
    },
  },
  {
    title: "Animated 'transform' prop value persisted",
    render: function (): React.Node {
      return (
        <RevertToStaticPropsExample type="timing" config={{duration: 1000}}>
          {value => {
            return (
              <Animated.View
                style={[
                  styles.block,
                  value
                    ? {
                        transform: [{translateX: value}],
                      }
                    : {},
                ]}
              />
            );
          }}
        </RevertToStaticPropsExample>
      );
    },
  },
];
