/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  ActivityIndicator,
  AppRegistry,
  ScrollView,
  StyleSheet,
  Switch,
  Text,
  TextInput,
  View,
} from 'react-native';
import {
  CheckBox,
  DatePicker,
  Picker,
} from 'react-native-windows';

export default class Bootstrap extends Component {
  constructor(props) {
    super(props);
    this.state = { focusable: true, hasBorder: false, radius: 0 };
  }

  render() {
    return (
      <View style={{ flex: 1, flexDirection: 'column', alignItems: 'center', justifyContent: 'center' }}>

        <CheckBox
          onValueChange={value => this.setState({ focusable: value })}
          checked={this.state.focusable} />
        <Text>acceptsKeyboardFocus</Text>

        <CheckBox
          onValueChange={value => this.setState({ hasBorder: value })}
          checked={this.state.hasBorder} />
        <Text>hasBorder</Text>

        <View enableFocusRing={true} acceptsKeyboardFocus={this.state.focusable ? true : false} style={!this.hasBorder ? styles.innerBorder : styles.noBorder}>
          <Text>The text!</Text>
        </View>
      </View>
    );
  }

  fullrender() {
    return (
      <View accessible={true} style={{ flex: 1, flexDirection: 'column', alignItems: 'center', justifyContent: 'center' }}>
        <View style={{ flexDirection: 'row', alignItems: 'center', marginTop: 15 }}>
          <CheckBox
            onValueChange={value => this.setState({ focusable: value })}
            checked={this.state.focusable} />
          <Text>acceptsKeyboardFocus</Text>
        </View>
        <View enableFocusRing={true} acceptsKeyboardFocus={this.state.focusable ? true : false} style={{ padding: 15, backgroundColor: 'lime', borderColor: 'navy', borderWidth: 2, borderRadius: 0 }}>
          <Text>TEXT</Text>
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <View style={{ height: 25, width: 75, backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }} />
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <DatePicker style={{ backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }} />
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <Picker style={{ backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }} />
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <Switch style={{ backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }} />
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <CheckBox style={{ backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }} />
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <TextInput style={{ backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }}
            placeholder={"type something ..."}
            placeholderTextColor={"maroon"}
          />
        </View>
        <View style={{ backgroundColor: 'orange', margin: 5 }}>
          <Text style={{ backgroundColor: 'lime', padding: this.state.padding, margin: this.state.margin }}>
            type nothing ...
          </Text>
        </View>
      </View>
    );
  }
};

const styles = StyleSheet.create({
  noBorder: {
    padding: 15,
    backgroundColor: 'orange'
  },
  innerBorder: {
    padding: 15,
    backgroundColor: 'lime',
    borderColor: 'navy',
    borderWidth: 1,
  },
  outerBorder: {
    padding: 15,
    backgroundColor: 'pink',
    borderColor: 'crimson',
    borderWidth: 1,
    borderRadius: 5,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
