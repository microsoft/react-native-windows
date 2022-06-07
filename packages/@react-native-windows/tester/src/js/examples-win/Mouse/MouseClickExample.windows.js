/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const React = require('react');

const {StyleSheet, View, Text, BackHandler, Button} = require('react-native');

exports.displayName = 'MouseClickExample';
exports.title = 'Mouse Click Events';
exports.category = 'Basic';
exports.description =
  'Tests that mouse click events work on intended components';
exports.examples = [
  {
    title: 'Mouse click events work on intended components\n',
    render: function (): React.Node {
      return <ExampleComponent />;
    },
  },
];

const styles = StyleSheet.create({
  page: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#FFFFFF',
  },
  mainContainer: {
    width: 400,
    height: 400,
    backgroundColor: '#CCCCCC',
  },
  logBoxStyle: {
    width: 250,
    padding: 10,
    marginBottom: 10,
    backgroundColor: '#f9f9f9',
  },
  viewStyle: {
    width: 100,
    height: 100,
    padding: 15,
    margin: 10,
    justifyContent: 'center',
    backgroundColor: '#add8e6',
    borderRadius: 50,
  },
});

export default class ExampleComponent extends React.Component<
  {},
  {
    primaryCount: number,
    secondaryCount: number,
    auxiliaryCount: number,
  },
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      primaryCount: 0,
      secondaryCount: 0,
      auxiliaryCount: 0,
    };
  }

  componentDidMount() {
    BackHandler.addEventListener('hardwareBackPress', this.back);
  }

  render() {
    const pageProps: any = {
      style: styles.page,
    };

    const clear = () => {
      this.setState({primaryCount: 0, auxiliaryCount: 0, secondaryCount: 0});
    };

    const incrementPrimary = () => {
      this.setState({primaryCount: this.state.primaryCount + 1});
    };

    const increment = (event: PressEvent) => {
      if (event.nativeEvent.button === 0) {
        this.setState({primaryCount: this.state.primaryCount + 1});
      } else if (event.nativeEvent.button === 1) {
        this.setState({auxiliaryCount: this.state.auxiliaryCount + 1});
      } else if (event.nativeEvent.button === 2) {
        this.setState({secondaryCount: this.state.secondaryCount + 1});
      }
    };

    return (
      <View style={styles.page} {...pageProps}>
        <View style={styles.logBoxStyle}>
          <Text testID="press_console_primary">
            Primary Pressed x{this.state.primaryCount}
          </Text>
          <Text testID="press_console_auxiliary">
            Auxiliary Pressed x{this.state.auxiliaryCount}
          </Text>
          <Text testID="press_console_secondary">
            Secondary Pressed x{this.state.secondaryCount}
          </Text>
        </View>
        <Button
          testID="clear_state_button"
          title="Clear state"
          onPress={clear}
        />
        <View style={styles.viewStyle} onStartShouldSetResponder={increment}>
          <Text testID="view_click">I'm a view!</Text>
        </View>
        <Button
          testID="button_click"
          title="I'm a button!"
          onPress={incrementPrimary}
        />
      </View>
    );
  }
}
