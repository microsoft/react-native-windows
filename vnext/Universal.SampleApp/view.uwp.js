/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

import React, {Component} from 'react';
import {
  AppRegistry,
  StyleSheet,
  Switch,
  Text,
  TextInput,
  View,
} from 'react-native';
import {CheckBox, DatePicker, Picker} from 'react-native-windows';

export default class Bootstrap extends Component {
  constructor(props) {
    super(props);
    this.state = {
      isFocusable: true,
      hasStyle: true,
      hasBorder: true,
      hasRadius: false,
      };
  }

  render() {
    const styles = StyleSheet.create({
      noBorder: {
        backgroundColor: 'orange',
      },
      innerBorder: {
        backgroundColor: 'lime',
        borderColor: 'navy',
        borderWidth: 1,
      },
      outerBorder: {
        backgroundColor: 'pink',
        borderColor: 'crimson',
        borderWidth: 1,
        borderRadius: 10,
      },
      radial: {
        backgroundColor: 'magenta',
        borderRadius: 10,
      },
    });

    return (
      <View
        style={{
          flex: 1,
          flexDirection: 'column',
          alignSelf: 'center',
          alignItems: 'center',
          justifyContent: 'center',
          paddingHorizontal: 20,
        }}>
        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({isFocusable: value})}
            checked={this.state.isFocusable}
          />
          <Text>acceptsKeyboardFocus</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({hasStyle: value})}
            checked={this.state.hasStyle}
          />
          <Text>hasStyle</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({hasBorder: value})}
            checked={this.state.hasBorder}
          />
          <Text>hasBorder</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({hasRadius: value})}
            checked={this.state.hasRadius}
          />
          <Text>hasRadius</Text>
        </View>

        <View style={{ flex: 1, flexDirection: 'column', alignItems: 'center', justifyContent: 'center', width: 250, backgroundColor: 'azure', }}>
          <View
            enableFocusRing={true}
            tabIndex={-1}
            acceptsKeyboardFocus={this.state.isFocusable ? true : false}
            style={
              this.state.hasStyle
                ? this.state.hasBorder
                  ? this.state.hasRadius
                    ? styles.outerBorder
                    : styles.innerBorder
                  : this.state.hasRadius
                    ? styles.radial
                    : styles.noBorder
                : null
            }>
            <Text>The text!</Text>
          </View>
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
        <View
          style={{flexDirection: 'row', alignItems: 'center', marginTop: 15}}>
          <CheckBox
            onValueChange={value => this.setState({isFocusable: value})}
            checked={this.state.isFocusable}
          />
          <Text>acceptsKeyboardFocus</Text>
        </View>
        <View
          enableFocusRing={true}
          acceptsKeyboardFocus={this.state.isFocusable ? true : false}
          style={{
            padding: 15,
            backgroundColor: 'lime',
            borderColor: 'navy',
            borderWidth: 2,
            borderRadius: 0,
          }}>
          <Text>TEXT</Text>
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
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
