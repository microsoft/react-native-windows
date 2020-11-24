/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  Button,
  StyleSheet,
  TextInput,
  Switch,
  View,
  Text,
  requireNativeComponent,
  FlatList,
} from 'react-native';

const FocusZoneView = requireNativeComponent('FocusZoneView');

const DATA = [
  {
    title: 'First Item',
  },
  {
    title: 'Second Item',
  },
  {
    title: 'Fourth Item',
  },
  {
    title: 'Fifth Item',
  },
  {
    title: 'Sixth Item',
  },
  {
    title: 'Seventh Item',
  },
  {
    title: 'Eighth Item',
  },
  {
    title: 'Nineth Item',
  },
  {
    title: 'Tenth Item',
  },
  {
    title: 'Eleventh Item',
  },
  {
    title: 'Twelfth Item',
  },
  {
    title: 'Thirteenth Item',
  },
  {
    title: 'Fourteenth Item',
  },
  {
    title: 'Fifteenth Item',
  },
  {
    title: 'Sixteenth Item',
  },
];

export default class Bootstrap extends React.Component<{}, any> {
  state = {
    passwordHidden: true,
    text: '',
    eventSwitchIsOn: false,
    eventSwitchRegressionIsOn: true,
  };

  render() {
    return (
      <>
        <View style={styles.container}>
          <TextInput
            style={styles.input}
            placeholder={'SpellChecking Disabled'}
            spellCheck={false}
          />
        </View>

        <FocusZoneView
          style={{
            height: 300,
            marginBottom: 100,
            flex: 1,
            alignItems: 'center',
          }}>
          <FlatList
            data={DATA}
            renderItem={({item}) => (
              <Button title={item.title} onPress={() => console.log('test')} />
            )}
          />
        </FocusZoneView>

        <FocusZoneView>
          <View style={{flexDirection: 'row', justifyContent: 'space-around'}}>
            <View>
              <Switch
                onValueChange={value => this.setState({eventSwitchIsOn: value})}
                style={{marginBottom: 10}}
                value={this.state.eventSwitchIsOn}
              />
              <Switch
                onValueChange={value => this.setState({eventSwitchIsOn: value})}
                style={{marginBottom: 10}}
                value={this.state.eventSwitchIsOn}
              />
              <Text>{this.state.eventSwitchIsOn ? 'On' : 'Off'}</Text>
            </View>
            <View>
              <Switch
                onValueChange={value =>
                  this.setState({eventSwitchRegressionIsOn: value})
                }
                style={{marginBottom: 10}}
                value={this.state.eventSwitchRegressionIsOn}
              />
              <Switch
                onValueChange={value =>
                  this.setState({eventSwitchRegressionIsOn: value})
                }
                style={{marginBottom: 10}}
                value={this.state.eventSwitchRegressionIsOn}
              />
              <Text>{this.state.eventSwitchRegressionIsOn ? 'On' : 'Off'}</Text>
            </View>
          </View>
        </FocusZoneView>

        <View style={styles.container}>
          <TextInput
            style={styles.input}
            placeholder={'SpellChecking Disabled'}
            spellCheck={false}
          />
        </View>
      </>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'center',
  },
  input: {
    margin: 5,
    width: 700,
    borderColor: '#7a42f4',
    borderWidth: 1,
    fontSize: 15,
  },
  item: {width: 500},
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
