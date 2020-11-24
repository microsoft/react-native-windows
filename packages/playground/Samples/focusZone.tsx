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
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
];

export default class Bootstrap extends React.Component<{}, any> {
  state = {
    passwordHidden: true,
    text: '',
    eventSwitchIsOn: false,
    eventSwitchRegressionIsOn: true,
  };

  Item = ({title}) => (
    <View style={styles.item}>
      <Text style={styles.title}>{title}</Text>
    </View>
  );

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
            keyExtractor={item => item.id}
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
