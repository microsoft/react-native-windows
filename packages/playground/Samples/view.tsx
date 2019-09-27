/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, CheckBox, StyleSheet, Text, View} from 'react-native';

export default class Bootstrap extends React.Component<
  {},
  {
    focusable: boolean;
    hasStyle: boolean;
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
      hasStyle: true,
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
      },
      innerBorder: {
        margin: 20,
        padding: 15,
        backgroundColor: 'lime',
        borderColor: 'navy',
        borderWidth: 1,
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
            value={this.state.focusable}
          />
          <Text>acceptsKeyboardFocus</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({hasStyle: value})}
            value={this.state.hasStyle}
          />
          <Text>hasStyle</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({hasBorder: value})}
            value={this.state.hasBorder}
          />
          <Text>hasBorder</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <CheckBox
            onValueChange={value => this.setState({radius: value})}
            value={this.state.radius}
          />
          <Text>hasRadius</Text>
        </View>

        <View
          style={{
            flex: 1,
            flexDirection: 'column',
            alignItems: 'center',
            justifyContent: 'center',
            width: 250,
            backgroundColor: 'azure',
          }}>
          <View
            acceptsKeyboardFocus={this.state.focusable ? true : false}
            style={
              this.state.hasStyle
                ? this.state.hasBorder
                  ? this.state.radius
                    ? styles.outerBorder
                    : styles.innerBorder
                  : this.state.radius
                    ? styles.radial
                    : styles.noBorder
                : null
            }
            {...{
              // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
              acceptsKeyboardFocus: true,
            }}>
            <Text>The text!</Text>
          </View>
        </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
