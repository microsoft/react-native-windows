/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 * @format
 */

import React, {Component} from 'react';
import {
  AppRegistry,
  ScrollView,
  Switch,
  Text,
  TextInput,
  View,
} from 'react-native';
import {CheckBox, DatePicker, Picker} from 'react-native-windows';

export default class Bootstrap extends Component {
  constructor(props) {
    super(props);
    this.state = {padding: 15, margin: 5};
  }

  render() {
    return (
      <View
        accessible={true}
        style={{
          flex: 1,
          flexDirection: 'column',
          alignItems: 'center',
          justifyContent: 'center',
        }}>
        <View style={{backgroundColor: 'orange', margin: 5, height: 100}}>
          <ScrollView
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}>
            <Text>first</Text>
            <Text>second</Text>
          </ScrollView>
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <View
            style={{
              height: 25,
              width: 75,
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <DatePicker
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <Picker
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <Switch
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <CheckBox
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <TextInput
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
            placeholder={'type something ...'}
            placeholderTextColor={'maroon'}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <Text
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}>
            type nothing ...
          </Text>
        </View>
      </View>
    );
  }

  fullrender() {
    return (
      <View
        accessible={true}
        style={{
          flex: 1,
          flexDirection: 'column',
          alignItems: 'center',
          justifyContent: 'center',
        }}>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <View
            style={{
              height: 25,
              width: 75,
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <DatePicker
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <Picker
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <Switch
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <CheckBox
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <TextInput
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}
            placeholder={'type something ...'}
            placeholderTextColor={'maroon'}
          />
        </View>
        <View style={{backgroundColor: 'orange', margin: 5}}>
          <Text
            style={{
              backgroundColor: 'lime',
              padding: this.state.padding,
              margin: this.state.margin,
            }}>
            type nothing ...
          </Text>
        </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
