/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, StyleSheet, Switch, Text, View} from 'react-native';

export default class Bootstrap extends React.Component<
  {},
  {
    focusable: boolean;
    hasStyle: boolean;
    hasBorder: boolean;
    radius: boolean;
    alignSelfCenter: boolean;
    largeBorder: boolean;
    largePadding: boolean;
  }
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      focusable: true,
      hasStyle: true,
      hasBorder: true,
      radius: true,
      alignSelfCenter: true,
      largeBorder: true,
      largePadding: true,
    };
  }

  render() {
    const styles = StyleSheet.create({
      noBorder: {
        width: 200,
        margin: 20,
        padding: this.state.largePadding ? 15 : 0,
        backgroundColor: 'orange',
      },
      innerBorder: {
        width: 200,
        margin: 20,
        padding: this.state.largePadding ? 15 : 0,
        backgroundColor: 'lime',
        borderColor: 'navy',
        borderWidth: this.state.largeBorder ? 20 : 1,
      },
      outerBorder: {
        width: 200,
        margin: 20,
        padding: this.state.largePadding ? 15 : 0,
        backgroundColor: 'pink',
        borderColor: 'crimson',
        borderWidth: this.state.largeBorder ? 20 : 1,
        borderRadius: 10,
      },
      radial: {
        width: 200,
        margin: 20,
        padding: this.state.largePadding ? 15 : 0,
        backgroundColor: 'magenta',
        borderRadius: 10,
        borderWidth: this.state.largeBorder ? 20 : 1,
      },
      child: {
        backgroundColor: 'yellow',
        alignSelf: this.state.alignSelfCenter ? 'center' : 'flex-start',
        width: 100,
        height: 50,
        fontSize: 20,
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
          <Switch
            onValueChange={value => this.setState({focusable: value})}
            value={this.state.focusable}
          />
          <Text>focusable</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <Switch
            onValueChange={value => this.setState({hasStyle: value})}
            value={this.state.hasStyle}
          />
          <Text>hasStyle</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <Switch
            onValueChange={value => this.setState({hasBorder: value})}
            value={this.state.hasBorder}
          />
          <Text>hasBorder</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <Switch
            onValueChange={value => this.setState({radius: value})}
            value={this.state.radius}
          />
          <Text>hasRadius</Text>
        </View>

        <View style={{flexDirection: 'row', alignSelf: 'flex-start'}}>
          <Switch
            onValueChange={value => this.setState({largeBorder: value})}
            value={this.state.largeBorder}
          />
          <Text>largeBorder</Text>
        </View>

        <View
          style={{flexDirection: 'row', alignSelf: 'flex-start', width: 100}}>
          <Switch
            onValueChange={value => this.setState({alignSelfCenter: value})}
            value={this.state.alignSelfCenter}
          />
          <Text>alignCenter</Text>
        </View>

        <View
          style={{flexDirection: 'row', alignSelf: 'flex-start', width: 100}}>
          <Switch
            onValueChange={value => this.setState({largePadding: value})}
            value={this.state.largePadding}
          />
          <Text>largePadding</Text>
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
            focusable={this.state.focusable ? true : false}
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
            }>
            <Text style={styles.child}>The Text</Text>
          </View>
        </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
