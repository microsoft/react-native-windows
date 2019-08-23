/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  StyleSheet,
  Switch,
  Text,
  TextInput,
  View,
} from 'react-native';
import {CheckBox, DatePicker, Picker} from 'react-native-windows';

export default class Bootstrap extends React.Component<
  {},
  {
    focusable: boolean;
    hasBorder: boolean;
    radius: boolean;
    padding: number;
    margin: number;
  }
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      focusable: true,
      hasBorder: true,
      radius: true,
      padding: 0,
      margin: 0,
    };
  }

  render() {
    const styles = StyleSheet.create({
      noBorder: {
        margin: 20,
        padding: 15,
        backgroundColor: 'orange',
        borderRadius: 0,
      },
      innerBorder: {
        margin: 20,
        padding: 15,
        backgroundColor: 'lime',
        borderColor: 'navy',
        borderWidth: 1,
        borderRadius: 0,
      },
      outerBorder: {
        margin: 20,
        padding: 15,
        backgroundColor: 'pink',
        borderColor: 'crimson',
        borderWidth: 1,
        borderRadius: 10,
      },
      radial: {
        margin: 20,
        padding: 15,
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
          width: 250,
          backgroundColor: 'azure',
          paddingHorizontal: 20,
        }}>
        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({focusable: value})}
            checked={this.state.focusable}
          />
          <Text>acceptsKeyboardFocus</Text>
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
            onValueChange={value => this.setState({radius: value})}
            checked={this.state.radius}
          />
          <Text>hasRadius</Text>
        </View>

        <View
          acceptsKeyboardFocus={this.state.focusable ? true : false}
          style={
            !this.state.hasBorder
              ? this.state.radius
                ? styles.radial
                : styles.noBorder
              : this.state.radius
                ? styles.outerBorder
                : styles.innerBorder
          }
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            acceptsKeyboardFocus: true,
          }}>
          <Text>The text!</Text>
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
            onValueChange={value => this.setState({focusable: value})}
            checked={this.state.focusable}
          />
          <Text>acceptsKeyboardFocus</Text>
        </View>
        <View
          acceptsKeyboardFocus={this.state.focusable ? true : false}
          style={{
            padding: 15,
            backgroundColor: 'lime',
            borderColor: 'navy',
            borderWidth: 2,
            borderRadius: 0,
          }}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            acceptsKeyboardFocus: true,
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
